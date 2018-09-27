#include "GoogleTest/GoogleTestFinder.h"

#include "Driver.h"
#include "Context.h"
#include "Config.h"
#include "ConfigParser.h"
#include "Mutators/MutatorsFactory.h"
#include "TestModuleFactory.h"
#include "GoogleTest/GoogleTest_Test.h"
#include "GoogleTest/GoogleTestRunner.h"
#include "Filter.h"
#include "MutationsFinder.h"
#include "Toolchain/Toolchain.h"
#include "Toolchain/JITEngine.h"

#include <llvm/IR/CallSite.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/YAMLTraits.h>
#include <llvm/Support/DynamicLibrary.h>

#include "gtest/gtest.h"

using namespace mull;
using namespace llvm;

static TestModuleFactory TestModuleFactory;

#pragma mark - Finding Tests

TEST(GoogleTestFinder, FindTest) {
  auto ModuleWithTests = TestModuleFactory.create_GoogleTest_Tester_Module();

  Context Ctx;
  Ctx.addModule(std::move(ModuleWithTests));

  const char *configYAML = R"YAML(
mutators:
- math_add_mutator
- negate_mutator
- remove_void_function_mutator
)YAML";
  
  yaml::Input Input(configYAML);
  
  ConfigParser Parser;
  auto config = Parser.loadConfig(Input);

  Filter filter;
  GoogleTestFinder Finder;

  auto tests = Finder.findTests(Ctx, filter);

  ASSERT_EQ(2U, tests.size());

  GoogleTest_Test *Test1 = dyn_cast<GoogleTest_Test>(tests[0].get());
  ASSERT_EQ("HelloTest.testSumOfTestee", Test1->getTestName());

  GoogleTest_Test *Test2 = dyn_cast<GoogleTest_Test>(tests[1].get());
  ASSERT_EQ("HelloTest.testSumOfTestee2", Test2->getTestName());
}

TEST(GoogleTestFinder, findTests_filter) {
  auto ModuleWithTests     = TestModuleFactory.create_GoogleTest_Tester_Module();

  Context Ctx;
  Ctx.addModule(std::move(ModuleWithTests));

  const char *configYAML = R"YAML(
mutators:
  - math_add_mutator
  - negate_mutator
  - remove_void_function_mutator
  )YAML";

  yaml::Input Input(configYAML);

  ConfigParser Parser;
  auto config = Parser.loadConfig(Input);

  Filter filter;
  filter.includeTest("HelloTest.testSumOfTestee");
  GoogleTestFinder Finder;

  auto tests = Finder.findTests(Ctx, filter);

  ASSERT_EQ(1U, tests.size());
  GoogleTest_Test *Test1 = dyn_cast<GoogleTest_Test>(tests[0].get());
  ASSERT_EQ("HelloTest.testSumOfTestee", Test1->getTestName());
}

TEST(DISABLED_GoogleTestRunner, runTest) {
  const char *configYAML = R"YAML(
mutators:
  - math_add_mutator
  - negate_mutator
  - remove_void_function_mutator
  )YAML";

  yaml::Input Input(configYAML);

  ConfigParser Parser;
  auto config = Parser.loadConfig(Input);

  Toolchain toolchain(config);

  auto moduleWithTests = TestModuleFactory.create_GoogleTest_Tester_Module();
  auto moduleWithTestees = TestModuleFactory.create_GoogleTest_Testee_Module();

  auto compiledModule_tests =
    toolchain.compiler().compileModule(moduleWithTests->getModule(), toolchain.targetMachine());
  auto compiledModule_testees = toolchain.compiler().compileModule(moduleWithTestees->getModule(), toolchain.targetMachine());

  Context Ctx;
  Ctx.addModule(std::move(moduleWithTests));
  Ctx.addModule(std::move(moduleWithTestees));

  Filter filter;
  GoogleTestFinder Finder;

  auto tests = Finder.findTests(Ctx, filter);

  ASSERT_EQ(2U, tests.size());

  GoogleTest_Test *Test1 = dyn_cast<GoogleTest_Test>(tests[0].get());
  ASSERT_EQ("HelloTest.testSumOfTestee", Test1->getTestName());

  GoogleTest_Test *Test2 = dyn_cast<GoogleTest_Test>(tests[1].get());
  ASSERT_EQ("HelloTest.testSumOfTestee2", Test2->getTestName());

  GoogleTestRunner runner(toolchain.mangler());
  JITEngine jit;

  std::vector<llvm::object::ObjectFile *> objects({
    compiledModule_tests.getBinary(),
    compiledModule_testees.getBinary()
  });
  /// TODO: enable eventually
//  runner.loadProgram(objects, jit);
  runner.runTest(Test1, jit);
}
