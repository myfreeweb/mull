#include "LLVMCompatibility.h"

using namespace llvm;

namespace llvm_compat {

uint64_t JITSymbolAddress(ORCJITSymbol &symbol) {
  return symbol.getAddress();
}

JITSymbolFlags JITSymbolFlagsFromObjectSymbol(const object::BasicSymbolRef &symbol) {
  return orc::JITSymbol::flagsFromObjectSymbol(symbol);
}

}

