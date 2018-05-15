#pragma once

#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/RuntimeDyld.h>

#include "LLVMCompatibility.h"

namespace mull {
class NativeResolver : public llvm_compat::ORCResolver {
  llvm::orc::LocalCXXRuntimeOverrides &overrides;
public:
  NativeResolver(llvm::orc::LocalCXXRuntimeOverrides &overrides);
  llvm_compat::ORCSymbolInfo findSymbol(const std::string &name) override;
  llvm_compat::ORCSymbolInfo findSymbolInLogicalDylib(const std::string &name) override;
};
}
