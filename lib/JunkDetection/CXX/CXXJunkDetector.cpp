#include "JunkDetection/CXX/CXXJunkDetector.h"
#include "MutationPoint.h"
#include "MutationOperator.h"

#include "Logger.h"

#include <iostream>

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Instruction.h>

using namespace mull;
using namespace llvm;
using namespace std;

ostream& operator<<(ostream& stream, const CXString& str) {
  stream << clang_getCString(str);
  clang_disposeString(str);
  return stream;
}

void dump_cursor(CXCursor cursor, CXSourceLocation location, PhysicalAddress &address) {
  CXCursorKind kind = clang_getCursorKind(cursor);

//  if (kind == CXCursor_CompoundStmt ||
//      kind == CXCursor_CXXForRangeStmt ||
//      kind == CXCursor_Constructor ||
//      kind == CXCursor_CallExpr ||
//      kind == CXCursor_CXXDeleteExpr ||
//      kind == CXCursor_ClassTemplate) {
//    return;
//  }

  cout << address.filepath << ":" << address.line << ":" << address.column << "\n";
  cout << "Kind '" << clang_getCursorKindSpelling(kind) << "'\n";

  if (kind == CXCursor_Namespace
//      || kind == CXCursor_CompoundStmt
      ) {
    cout << "not printing contents\n";
    return;
  }

  CXSourceRange range = clang_getCursorExtent(cursor);
  CXSourceLocation begin = clang_getRangeStart(range);
  CXSourceLocation end = clang_getRangeEnd(range);

  unsigned int beginOffset = 0;
  unsigned int endOffset = 0;
  unsigned int origOffset = 0;
  clang_getFileLocation(begin, nullptr, nullptr, nullptr, &beginOffset);
  clang_getFileLocation(end, nullptr, nullptr, nullptr, &endOffset);
  clang_getFileLocation(location, nullptr, nullptr, nullptr, &origOffset);

  unsigned int offset = origOffset - beginOffset;

  auto length = endOffset - beginOffset;

  FILE *f = fopen(address.filepath.c_str(), "rb");

  fseek(f, beginOffset, SEEK_SET);
  char *buffer = new char[length + 1];
  fread(buffer, sizeof(char), length, f);

  buffer[length] = '\0';
  cout << buffer << "\n";

  for (unsigned int i = 0; i < offset; i++) {
    printf(" ");
  }
  printf("^\n");

  delete[] buffer;
}

PhysicalAddress getAddress(MutationPoint *point) {
  PhysicalAddress address;

  if (auto instruction = dyn_cast<Instruction>(point->getOriginalValue())) {
    if (instruction->getMetadata(0)) {
      auto debugInfo = instruction->getDebugLoc();
      address.filepath = debugInfo->getFilename().str();
      address.line = debugInfo->getLine();
      address.column = debugInfo->getColumn();
    }
  }

  return address;
}

CXXJunkDetector::CXXJunkDetector() : index(clang_createIndex(true, true)) {

}

CXXJunkDetector::~CXXJunkDetector() {
  for (auto &pair : units) {
    clang_disposeTranslationUnit(pair.second);
  }
  clang_disposeIndex(index);
}

pair<CXCursor, CXSourceLocation> CXXJunkDetector::cursorAndLocation(PhysicalAddress &address) {
  if (units.count(address.filepath) == 0) {
    const char *argv[] = {
      "-x", "c++", "-std=c++11",
      "-DGTEST_HAS_TR1_TUPLE=0", "-DGTEST_LANG_CXX11=1", "-DGTEST_HAS_RTTI=0",
      "-D__STDC_CONSTANT_MACROS", "-D__STDC_FORMAT_MACROS", "-D__STDC_LIMIT_MACROS",
      "-I/Users/alexdenisov/Projects/LLVM/llvm/include",
      "-I/Users/alexdenisov/Projects/LLVM/build/include/",
      "-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk/usr/include/libxml2",
      "-I/Users/alexdenisov/Projects/LLVM/llvm/utils/unittest/googletest/include",
      "-I/Users/alexdenisov/Projects/LLVM/llvm/utils/unittest/googlemock/include",
      nullptr };
    const int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    CXTranslationUnit unit = clang_parseTranslationUnit(index,
                                                        address.filepath.c_str(),
                                                        argv, argc,
                                                        nullptr, 0,
                                                        CXTranslationUnit_KeepGoing);
    if (unit == nullptr) {
      Logger::error() << "Cannot parse translation unit: " << address.filepath << "\n";
      return make_pair(clang_getNullCursor(), clang_getNullLocation());
    }
    units[address.filepath] = unit;
  }

  CXTranslationUnit unit = units[address.filepath];
  if (unit == nullptr) {
    return make_pair(clang_getNullCursor(), clang_getNullLocation());
  }

  CXFile file = clang_getFile(unit, address.filepath.c_str());
  if (file == nullptr) {
    Logger::error() << "Cannot get file from TU: " << address.filepath << "\n";
    return make_pair(clang_getNullCursor(), clang_getNullLocation());
  }

  CXSourceLocation location = clang_getLocation(unit, file, address.line, address.column);

  return make_pair(clang_getCursor(unit, location), location);
}

bool CXXJunkDetector::isJunk(MutationPoint *point) {
  auto address = getAddress(point);
  if (!address.valid()) {
    return true;
  }

  auto pair = cursorAndLocation(address);
  auto cursor = pair.first;
  auto location = pair.second;

  if (clang_Cursor_isNull(cursor)) {
    return true;
  }

  bool junk = false;

  switch (point->getOperator()->getKind()) {
    case MutationOperatorKind::MathAdd:
      junk = junkMathAdd(cursor, location, address);
      break;
//    case MutationOperatorKind::NegateCondition:
//      junk = junkNegateCondition(cursor, location, address);
//      break;
    case MutationOperatorKind::RemoveVoidFunctionCall:
      junk = junkRemoveVoidFunctionCall(cursor, location, address);
      break;

    default:
      Logger::debug() << "CXXJunkDetector does not work with " << point->getOperator()->uniqueID() << " yet.\n";
      break;
  }

  if (junk) {
    cout << point->getUniqueIdentifier() << "\n";
    point->getOriginalValue()->dump();
  }

  return junk;
}

bool CXXJunkDetector::junkMathAdd(CXCursor cursor, CXSourceLocation location, PhysicalAddress &address) {
  CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind != CXCursor_BinaryOperator &&
      kind != CXCursor_UnaryOperator &&
      kind != CXCursor_CompoundAssignOperator &&
      kind != CXCursor_OverloadedDeclRef) {
    dump_cursor(cursor, location, address);
    return true;
  }
  unsigned int mutationOffset = 0;
  clang_getFileLocation(location, nullptr, nullptr, nullptr, &mutationOffset);
  FILE *f = fopen(address.filepath.c_str(), "rb");
  char symbol;
  fseek(f, mutationOffset, SEEK_SET);
  fread(&symbol, sizeof(char), 1, f);
  fclose(f);

  if (symbol != '+' && symbol != '-') {
    dump_cursor(cursor, location, address);
    return true;
  }

  return false;
}

bool CXXJunkDetector::junkNegateCondition(CXCursor cursor, CXSourceLocation location, PhysicalAddress &address) {
  /// Junk:
  ///
  ///   CompoundStmt
  ///   CXXConstructor
  ///   CXXForRangeStmt
  ///   CallExpr
  ///   CXXDeleteExpr
  ///   ClassTemplate
  ///   ForStmt
  ///

  /// Maybe Junk
  ///
  ///   UnexposedDecl
  ///   IfStmt
  ///   VarDecl
  ///   MemberRefExpr
  ///

  CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind != CXCursor_BinaryOperator &&
      kind != CXCursor_UnaryOperator &&
      kind != CXCursor_DeclRefExpr &&
      kind != CXCursor_TypeRef &&
      kind != CXCursor_ParenExpr &&
      kind != CXCursor_CompoundAssignOperator &&
      kind != CXCursor_OverloadedDeclRef &&
      kind != CXCursor_UnexposedDecl &&
      kind != CXCursor_IfStmt) {
    dump_cursor(cursor, location, address);
    return true;
  }

  return false;
}

bool CXXJunkDetector::junkRemoveVoidFunctionCall(CXCursor cursor,
                                                 CXSourceLocation location,
                                                 PhysicalAddress &address) {
  /// Junk
  ///
  ///   CompoundStmt <- MACROS ARE EVIL
  ///   CXXDeleteExpr
  ///

  /// Probably Junk
  ///
  ///   CallExpr
  ///   DeclRefExpr
  ///   DefaultStmt
  ///

  /// Probably Not Junk
  ///
  ///   TypeRef
  ///   NamespaceRef
  ///

  /// Not Junk
  ///
  ///   MemberRefExpr
  ///

  CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind != CXCursor_MemberRefExpr
//      && kind != CXCursor_UnaryOperator
//      && kind != CXCursor_CompoundAssignOperator
//      && kind != CXCursor_OverloadedDeclRef
      ) {
    return false;
  }

  dump_cursor(cursor, location, address);
  return false;
}
