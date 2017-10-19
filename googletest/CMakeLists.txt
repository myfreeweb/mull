message(STATUS "googletest> Creating google-test target...")

set(google_test_include_dirs
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories(${google_test_include_dirs})

file(GLOB_RECURSE google_test_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(google_test_sources src/gtest-all.cc)
set(google_test_main_sources src/gtest_main.cc)

add_library(google-test SHARED ${google_test_sources} ${google_test_headers})
add_library(google-test-main SHARED ${google_test_main_sources} ${google_test_headers})

# compile flags
get_target_property(default_compile_flags google-test COMPILE_FLAGS)
if(NOT default_compile_flags)
  set(default_compile_flags "")
endif()
set(googletest_compileflags ${default_compile_flags} ${LLVM_CXX_FLAGS})
set_target_properties(google-test
  PROPERTIES COMPILE_FLAGS
  "${googletest_compileflags}"
)
set_target_properties(google-test-main
  PROPERTIES COMPILE_FLAGS
  "${googletest_compileflags}"
)

get_target_property(default_link_flags google-test LINK_FLAGS)
if(NOT default_link_flags)
  set(default_link_flags "")
endif()
set(googletest_linkflags ${default_link_flags} ${LLVM_LINK_FLAGS})
set_target_properties(google-test
  PROPERTIES LINK_FLAGS
  "${googletest_linkflags}"
)
