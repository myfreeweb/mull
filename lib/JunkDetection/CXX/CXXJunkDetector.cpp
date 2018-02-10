#include "JunkDetection/CXX/CXXJunkDetector.h"
#include "MutationPoint.h"

using namespace mull;

CXXJunkDetector::CXXJunkDetector() : index(clang_createIndex(true, true)) {

}

CXXJunkDetector::~CXXJunkDetector() {
  for (auto &pair : units) {
    clang_disposeTranslationUnit(pair.second);
  }
  clang_disposeIndex(index);
}

bool CXXJunkDetector::isJunk(MutationPoint *point) {
  return false;
}
