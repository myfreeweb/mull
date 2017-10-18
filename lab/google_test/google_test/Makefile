MAKEFLAGS=--warn-undefined-variables

CC=/opt/llvm-3.9/bin/clang
# CC=xcrun -sdk macosx clang

BUILD_DIR=build
GTEST_DIR=../../../googletest

HEADER_SEARCH_FLAGS=-I$(GTEST_DIR) -I$(GTEST_DIR)/include
MACRO_DEFINITONS=-DGTEST_NO_LLVM_RAW_OSTREAM=1
STD_VERSION=-std=c++11

# This also depends on min mac version of google test.
MACOSX_VERSION=-mmacosx-version-min=10.12
SYSROOT=-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk

NO_CXA=
#NO_CXA=-fno-use-cxa-atexit

CXXFLAGS:=$(shell /opt/llvm-3.9/bin/llvm-config --cxxflags)  -g
CXXFLAGS:=$(subst -O3,-O0,$(CXXFLAGS))

CC_FLAGS=$(CXXFLAGS) $(STD_VERSION) $(HEADER_SEARCH_FLAGS) $(MACRO_DEFINITONS) $(MACOSX_VERSION) $(SYSROOT) $(NO_CXA)

OBJECTS= $(BUILD_DIR)/Test.o \
        $(BUILD_DIR)/Testee.o

LLVM_IR=$(BUILD_DIR)/Test.ll \
        $(BUILD_DIR)/Testee.ll

BITCODE=$(BUILD_DIR)/Test.bc \
        $(BUILD_DIR)/Testee.bc

all: clean llvm_ir bitcode

llvm_ir: $(BUILD_DIR) $(LLVM_IR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

bitcode: $(BITCODE)

driver: $(BUILD_DIR) $(BUILD_DIR)/tests

run: driver
	$(BUILD_DIR)/tests

$(BUILD_DIR):
	mkdir $@

## Bitcode

$(BUILD_DIR)/%.bc:
	$(CC) -emit-llvm -c $(CC_FLAGS) ./$*.cpp -o $@

## LLVM IR

$(BUILD_DIR)/%.ll:
	$(CC) -S -emit-llvm -c $(CC_FLAGS) ./$*.cpp -o $@

## Mach-o

$(BUILD_DIR)/%.o:
	$(CC) -c $(CC_FLAGS) ./$*.cpp -o $@

# -stdlib=libc++ comes from https://stackoverflow.com/a/16353112/598057
$(BUILD_DIR)/tests: $(OBJECTS)
	$(CC)++ -stdlib=libc++ -L$(GTEST_DIR) -L/opt/mull/BuildXcode/googletest/Debug/ -lgoogle-test -lgoogle-test-main $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
