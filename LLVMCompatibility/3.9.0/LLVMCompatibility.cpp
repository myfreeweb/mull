#include "LLVMCompatibility.h"

using namespace llvm;

namespace llvm_compat {

JITSymbolFlags JITSymbolFlagsFromObjectSymbol(const object::BasicSymbolRef &symbol) {
  return orc::JITSymbol::flagsFromObjectSymbol(symbol);
}

}

