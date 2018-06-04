
#include <llvm/ExecutionEngine/RuntimeDyld.h>

#include <llvm/Bitcode/BitcodeReader.h>

namespace llvm_compat {
  using namespace llvm;

  typedef JITSymbolResolver ORCResolver;
  typedef JITSymbol ORCSymbolInfo;
  typedef JITSymbol ORCJITSymbol;

  JITSymbolFlags JITSymbolFlagsFromObjectSymbol(const object::BasicSymbolRef &symbol);
}
