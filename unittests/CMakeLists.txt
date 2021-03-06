set(mull_unittests_sources
  CompilerTests.cpp
  ConfigParserTests.cpp
  ContextTest.cpp
  DriverTests.cpp
  ForkProcessSandboxTest.cpp
  MutationPointTests.cpp
  ModuleLoaderTest.cpp
  DynamicCallTreeTests.cpp
  MutatorsFactoryTests.cpp
  TesteesTests.cpp

  TestRunnersTests.cpp
  UniqueIdentifierTests.cpp
  TaskExecutorTests.cpp

  Mutators/MutatorsTests.cpp
  Mutators/NegateConditionMutatorTest.cpp
  Mutators/RemoveVoidFunctionMutatorTest.cpp
  Mutators/ScalarValueMutatorTest.cpp
  Mutators/ConditionalsBoundaryMutatorTests.cpp

  JunkDetection/CXXJunkDetectorTests.cpp

  SimpleTest/SimpleTestFinderTest.cpp

  GoogleTest/GoogleTestFinderTest.cpp

  CustomTestFramework/CustomTestRunnerTests.cpp
  CustomTestFramework/CustomTestFinderTests.cpp

  SQLiteReporterTest.cpp

  TestModuleFactory.cpp
  TestModuleFactory.h

  ConfigParserTestFixture.h
)

add_executable(MullUnitTests ${mull_unittests_sources})
target_link_libraries(MullUnitTests
  mull
  google-test
)
target_include_directories(MullUnitTests PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${MULL_SOURCE_DIR}/googletest/include
  ${MULL_INCLUDE_DIRS}
)
set_target_properties(MullUnitTests PROPERTIES
  COMPILE_FLAGS ${MULL_CXX_FLAGS}
  LINK_FLAGS ${MULL_LINK_FLAGS}
)

# This copies (symlinks) fixtures/*.bc files that used by unit tests.
add_custom_command(TARGET MullUnitTests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_CURRENT_LIST_DIR}/fixtures $<TARGET_FILE_DIR:MullUnitTests>/fixtures)

