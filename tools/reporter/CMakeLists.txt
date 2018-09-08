set(llvm_components "support")
llvm_get_libs(llvm_libraries ${llvm_components})
string(REPLACE ";" " " llvm_libraries "${llvm_libraries}")
add_executable(mull-reporter
  WeakTestsReporter.h
  WeakTestsReporter.cpp

  reporter.cpp
)
set_target_properties(mull-reporter PROPERTIES
  LINK_FLAGS "${LLVM_LINK_FLAGS}"
  COMPILE_FLAGS "-fno-rtti -D_GLIBCXX_USE_CXX11_ABI=0"
)
target_link_libraries(mull-reporter
  sqlite3
  pthread
  tinfo
  ${llvm_libraries}
)

INSTALL(TARGETS mull-reporter
  RUNTIME DESTINATION bin
)

