
#include <llvm/ExecutionEngine/RuntimeDyld.h>

#include <llvm/Bitcode/BitcodeReader.h>

namespace llvm_compat {
  typedef llvm::JITSymbolResolver ORCResolver;
  typedef llvm::JITSymbol ORCSymbolInfo;
  typedef llvm::JITSymbol ORCJITSymbol;
}
