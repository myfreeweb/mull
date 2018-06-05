#pragma once

#include "TestRunner.h"

#include "Mangler.h"

#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
//#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/Object/Binary.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/Target/TargetMachine.h>
#include <Toolchain/Resolvers/NativeResolver.h>

namespace llvm {

class Function;

}

namespace mull {

struct InstrumentationInfo;

class Machine {
  std::vector<llvm::object::ObjectFile *> objectFiles;
  llvm::SectionMemoryManager memoryManager;
  llvm::StringMap<llvm_compat::ORCJITSymbol> symbolTable;
public:
  void addObjectFiles(std::vector<llvm::object::ObjectFile *> &files,
                      llvm_compat::ORCResolver &resolver);
  llvm_compat::ORCJITSymbol &getSymbol(llvm::StringRef name);
};

class GoogleTestRunner : public TestRunner {
//  llvm::orc::ObjectLinkingLayer<> ObjectLayer;
  Mangler mangler;
  llvm::orc::LocalCXXRuntimeOverrides overrides;

  std::string fGoogleTestInit;
  std::string fGoogleTestInstance;
  std::string fGoogleTestRun;
//  llvm::orc::ObjectLinkingLayer<>::ObjSetHandleT handle;
  InstrumentationInfo **trampoline;
  Machine machine;
public:

  GoogleTestRunner(llvm::TargetMachine &machine);
  ~GoogleTestRunner();

  void loadInstrumentedProgram(ObjectFiles &objectFiles, Instrumentation &instrumentation) override;
  void loadProgram(ObjectFiles &objectFiles) override;
  ExecutionStatus runTest(Test *test) override;

private:
  void *GetCtorPointer(const llvm::Function &Function);
  void *getFunctionPointer(const std::string &functionName);

  void runStaticCtor(llvm::Function *Ctor);
};

}
