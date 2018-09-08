set (SOURCES
  LLVMCompatibility.cpp
  LLVMCompatibility.h
)

add_library(LLVMCompatibility OBJECT ${SOURCES})
set_target_properties(LLVMCompatibility
	PROPERTIES COMPILE_FLAGS "-fno-rtti -D_GLIBCXX_USE_CXX11_ABI=0")

