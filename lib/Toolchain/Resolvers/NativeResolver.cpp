#include "Toolchain/Resolvers/NativeResolver.h"

#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

using namespace mull;
using namespace llvm;

NativeResolver::NativeResolver(llvm::orc::LocalCXXRuntimeOverrides &overrides)
: overrides(overrides) {}

llvm_compat::ORCSymbolInfo NativeResolver::findSymbol(const std::string &name) {
  /// Overrides should go first, otherwise functions of the host process
  /// will take over and crash the system later
  if (auto symbol = overrides.searchOverrides(name)) {
    return symbol;
  }

  if (auto address = RTDyldMemoryManager::getSymbolAddressInProcess(name)) {
    return llvm_compat::ORCSymbolInfo(address, JITSymbolFlags::Exported);
  }

  return llvm_compat::ORCSymbolInfo(nullptr);
}

llvm_compat::ORCSymbolInfo NativeResolver::findSymbolInLogicalDylib(const std::string &name) {
  return llvm_compat::ORCSymbolInfo(nullptr);
}
