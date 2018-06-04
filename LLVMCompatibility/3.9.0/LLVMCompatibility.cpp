#include "LLVMCompatibility.h"

using namespace llvm_compat;
using namespace llvm;

JITSymbolFlags JITSymbolFlagsFromObjectSymbol(const object::BasicSymbolRef &symbol) {
  return JITSymbol::flagsFromObjectSymbol(symbol);
}

