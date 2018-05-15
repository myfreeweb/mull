#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/ExecutionEngine/Orc/JITSymbol.h>

#include <llvm/Bitcode/ReaderWriter.h>

namespace llvm_compat {
  typedef llvm::RuntimeDyld::SymbolResolver ORCResolver;
  typedef llvm::RuntimeDyld::SymbolInfo ORCSymbolInfo;
  typedef llvm::orc::JITSymbol ORCJITSymbol;

}
