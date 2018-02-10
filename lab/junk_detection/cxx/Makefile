CC=/opt/llvm-3.9/bin/clang
CXX=/opt/llvm-3.9/bin/clang++
LEVEL=../../..
BUILD_DIR=./build
FIXTURES_DIR=$(LEVEL)/unittests/fixtures/junk_detection/cxx/

BITCODE_FILES=math_c.bc math_cpp.bc

bitcode: $(BUILD_DIR) $(BITCODE_FILES)

%.bc: %.c
	$(CC) -g -c -emit-llvm $(PWD)/$*.c -o $(BUILD_DIR)/$*.bc

%.bc: %.cpp
	$(CXX) -g -c -emit-llvm $(PWD)/$*.cpp -o $(BUILD_DIR)/$*.bc

synchronize_fixtures: bitcode $(FIXTURES_DIR)
	cp $(BUILD_DIR)/*.bc $(FIXTURES_DIR)

$(FIXTURES_DIR):
	mkdir -p $(FIXTURES_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

