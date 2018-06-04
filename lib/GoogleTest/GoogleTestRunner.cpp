#include "GoogleTest/GoogleTestRunner.h"

#include "GoogleTest/GoogleTest_Test.h"
#include "Mangler.h"

#include "Toolchain/Resolvers/InstrumentationResolver.h"
#include "Toolchain/Resolvers/NativeResolver.h"

#include <llvm/IR/Function.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

using namespace mull;
using namespace llvm;
using namespace llvm::orc;

namespace {
  class UnitTest;
}

static llvm::orc::ObjectLinkingLayer<>::ObjSetHandleT MullGTEstDummyHandle;

GoogleTestRunner::GoogleTestRunner(llvm::TargetMachine &machine) :
  TestRunner(machine),
  mangler(Mangler(machine.createDataLayout())),
  overrides([this](const char *name) {
    return this->mangler.getNameWithPrefix(name);
  }),
  fGoogleTestInit(mangler.getNameWithPrefix("_ZN7testing14InitGoogleTestEPiPPc")),
  fGoogleTestInstance(mangler.getNameWithPrefix("_ZN7testing8UnitTest11GetInstanceEv")),
  fGoogleTestRun(mangler.getNameWithPrefix("_ZN7testing8UnitTest3RunEv")),
  handle(MullGTEstDummyHandle),
  trampoline(new InstrumentationInfo*)
{
}

GoogleTestRunner::~GoogleTestRunner() {
  delete trampoline;
}

void *GoogleTestRunner::GetCtorPointer(const llvm::Function &Function) {
  return
    getFunctionPointer(mangler.getNameWithPrefix(Function.getName().str()));
}

void *GoogleTestRunner::getFunctionPointer(const std::string &functionName) {
  //  JITSymbol symbol = ObjectLayer.findSymbol(functionName, false);
  JITSymbol symbol = machine.getSymbol(functionName);

  void *fpointer =
    reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.getAddress()));

  if (fpointer == nullptr) {
    errs() << "GoogleTestRunner> Can't find pointer to function: "
           << functionName << "\n";
    exit(1);
  }

  return fpointer;
}

void GoogleTestRunner::runStaticCtor(llvm::Function *Ctor) {
//  printf("Init: %s\n", Ctor->getName().str().c_str());

  void *CtorPointer = GetCtorPointer(*Ctor);

  auto ctor = ((int (*)())(intptr_t)CtorPointer);
  ctor();
}

void GoogleTestRunner::loadInstrumentedProgram(ObjectFiles &objectFiles,
                                               Instrumentation &instrumentation) {
//  machine.addObjectFiles(objectFiles);
//
//  if (handle != MullGTEstDummyHandle) {
//    ObjectLayer.removeObjectSet(handle);
//  }
//
//  handle = ObjectLayer.addObjectSet(objectFiles,
//                                    make_unique<SectionMemoryManager>(),
//                                    make_unique<InstrumentationResolver>(overrides,
//                                                                         instrumentation,
//                                                                         mangler,
//                                                                         trampoline));
  InstrumentationResolver resolver(overrides, instrumentation, mangler, trampoline);
//  ObjectLayer.emitAndFinalize(handle);
  machine.addObjectFiles(objectFiles, resolver);
}

void GoogleTestRunner::loadProgram(ObjectFiles &objectFiles) {
//  if (handle != MullGTEstDummyHandle) {
//    ObjectLayer.removeObjectSet(handle);
//  }
//
//  handle = ObjectLayer.addObjectSet(objectFiles,
//                                    make_unique<SectionMemoryManager>(),
//                                    make_unique<NativeResolver>(overrides));
//  ObjectLayer.emitAndFinalize(handle);

  NativeResolver resolver(overrides);
  machine.addObjectFiles(objectFiles, resolver);
}

ExecutionStatus GoogleTestRunner::runTest(Test *test) {
  *trampoline = &test->getInstrumentationInfo();

  GoogleTest_Test *GTest = dyn_cast<GoogleTest_Test>(test);

  for (auto &Ctor: GTest->GetGlobalCtors()) {
    runStaticCtor(Ctor);
  }

  std::string filter = "--gtest_filter=" + GTest->getTestName();
  const char *argv[] = { "mull", filter.c_str(), NULL };
  int argc = 2;

  /// Normally Google Test Driver looks like this:
  ///
  ///   int main(int argc, char **argv) {
  ///     InitGoogleTest(&argc, argv);
  ///     return UnitTest.GetInstance()->Run();
  ///   }
  ///
  /// Technically we can just call `main` function, but there is a problem:
  /// Among all the files that are being processed may be more than one
  /// `main` function, therefore we can call wrong driver.
  ///
  /// To avoid this from happening we implement the driver function on our own.
  /// We must keep in mind that each project can have its own, extended
  /// version of the driver (LLVM itself has one).
  ///

  void *initGTestPtr = getFunctionPointer(fGoogleTestInit);

  auto initGTest = ((void (*)(int *, const char**))(intptr_t)initGTestPtr);
  initGTest(&argc, argv);

  void *getInstancePtr = getFunctionPointer(fGoogleTestInstance);

  auto getInstance = ((UnitTest *(*)())(intptr_t)getInstancePtr);
  UnitTest *unitTest = getInstance();

  void *runAllTestsPtr = getFunctionPointer(fGoogleTestRun);

  auto runAllTests = ((int (*)(UnitTest *))(intptr_t)runAllTestsPtr);
  uint64_t result = runAllTests(unitTest);

  overrides.runDestructors();

  if (result == 0) {
    return ExecutionStatus::Passed;
  }
  return ExecutionStatus::Failed;
}

void Machine::addObjectFiles(std::vector<llvm::object::ObjectFile *> &files,
                             llvm_compat::ORCResolver &resolver) {
  std::vector<llvm::object::ObjectFile *>().swap(objectFiles);
  llvm::StringMap<llvm_compat::ORCSymbolInfo>().swap(symbolTable);

  for (auto object : files) {
    objectFiles.push_back(object);

    for (auto symbol : object->symbols()) {
      if (symbol.getFlags() & object::SymbolRef::SF_Undefined) {
        continue;
      }

      Expected<StringRef> name = symbol.getName();
      if (!name) {
        consumeError(name.takeError());
        continue;
      }

      auto flags = llvm_compat::JITSymbolFlagsFromObjectSymbol(symbol);
      symbolTable.insert(std::make_pair(name.get(), llvm_compat::ORCJITSymbol(0, flags)));
    }

  }

  RuntimeDyld dynamicLoader(memoryManager, resolver);
  dynamicLoader.setProcessAllSections(false);

  for (auto &object : objectFiles) {
    dynamicLoader.loadObject(*object);
  }

  for (auto &entry : symbolTable) {
    entry.second = dynamicLoader.getSymbol(entry.first());
  }

  dynamicLoader.finalizeWithMemoryManagerLocking();

//  void buildInitialSymbolTable(const ObjSetT &Objects) {
//    for (const auto &Obj : Objects)
//      for (auto &Symbol : getObject(*Obj).symbols()) {
//        if (Symbol.getFlags() & object::SymbolRef::SF_Undefined)
//          continue;
//        Expected<StringRef> SymbolName = Symbol.getName();
//        // FIXME: Raise an error for bad symbols.
//        if (!SymbolName) {
//          consumeError(SymbolName.takeError());
//          continue;
//        }
//        auto Flags = JITSymbol::flagsFromObjectSymbol(Symbol);
//        SymbolTable.insert(
//            std::make_pair(*SymbolName, RuntimeDyld::SymbolInfo(0, Flags)));
//      }
//  }

}


llvm_compat::ORCJITSymbol Machine::getSymbol(StringRef name) {
  return symbolTable.find(name)->second;
}

//void Machine::finalize() {
//  RuntimeDyld dynamicLoader(memoryManager, resolver);
//  dynamicLoader.setProcessAllSections(false);
//
//  for (auto &object : objectFiles) {
//    dynamicLoader.loadObject(*object);
//  }
//
//  for (auto &entry : symbolTable) {
//    entry.second = dynamicLoader.getSymbol(entry.first());
//  }
//
//  dynamicLoader.finalizeWithMemoryManagerLocking();
//
//  //  auto Finalizer = [&](ObjSetHandleT H, RuntimeDyld &RTDyld,
////                       const ObjSetT &Objs,
////                       std::function<void()> LOSHandleLoad) {
////    LoadedObjInfoList LoadedObjInfos;
////
////    for (auto &Obj : Objs)
////      LoadedObjInfos.push_back(RTDyld.loadObject(this->getObject(*Obj)));
////
////    LOSHandleLoad();
////
////    this->NotifyLoaded(H, Objs, LoadedObjInfos);
////
////    RTDyld.finalizeWithMemoryManagerLocking();
////
////    if (this->NotifyFinalized)
////      this->NotifyFinalized(H);
////  };;
//
////  RuntimeDyld RTDyld(*MemMgr, *PFC->Resolver);
////  RTDyld.setProcessAllSections(PFC->ProcessAllSections);
////  PFC->RTDyld = &RTDyld;
////
////  PFC->Finalizer(PFC->Handle, RTDyld, std::move(PFC->Objects),
////                 [&]() {
////                   this->updateSymbolTable(RTDyld);
////                   this->Finalized = true;
////                 });
//
////  void updateSymbolTable(const RuntimeDyld &RTDyld) {
////    for (auto &SymEntry : SymbolTable)
////      SymEntry.second = RTDyld.getSymbol(SymEntry.first());
////  }
//
//}
//
//Machine::Machine(TargetMachine &tm) :
//    mangler(Mangler(tm.createDataLayout())),
//    overrides([this](const char *name) {
//      return this->mangler.getNameWithPrefix(name);
//    }),
//    resolver(overrides)
//{
//}
