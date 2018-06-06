#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/ExecutionEngine/Orc/JITSymbol.h>
#include <llvm/Bitcode/ReaderWriter.h>

namespace llvm_compat {
  using namespace llvm;

  typedef RuntimeDyld::SymbolResolver ORCResolver;
  typedef RuntimeDyld::SymbolInfo ORCSymbolInfo;
  typedef orc::JITSymbol ORCJITSymbol;

  uint64_t JITSymbolAddress(ORCJITSymbol &symbol);
  JITSymbolFlags JITSymbolFlagsFromObjectSymbol(const object::BasicSymbolRef &symbol);
}
