#pragma once

#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <string>

#include "LLVMCompatibility.h"

namespace llvm {
namespace orc {
  class LocalCXXRuntimeOverrides;
}
}

namespace mull {

class Mangler;
class Instrumentation;
struct InstrumentationInfo;

class InstrumentationResolver : public llvm_compat::ORCResolver {
  llvm::orc::LocalCXXRuntimeOverrides &overrides;
  Instrumentation &instrumentation;
  std::string instrumentationInfoName;
  std::string functionOffsetPrefix;
  InstrumentationInfo **trampoline;
public:
  InstrumentationResolver(llvm::orc::LocalCXXRuntimeOverrides &overrides,
                          Instrumentation &instrumentation,
                          mull::Mangler &mangler,
                          InstrumentationInfo **trampoline);

  llvm_compat::ORCSymbolInfo findSymbol(const std::string &name) override;
  llvm_compat::ORCSymbolInfo findSymbolInLogicalDylib(const std::string &name) override;
};
}
