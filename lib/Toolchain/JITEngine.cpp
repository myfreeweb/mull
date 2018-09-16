
#include "Toolchain/JITEngine.h"

using namespace mull;
using namespace llvm;

JITEngine::JITEngine() : symbolNotFound(nullptr) {}

void JITEngine::addObjectFiles(std::vector<object::ObjectFile *> &files,
                               llvm_compat::SymbolResolver &resolver,
                               std::unique_ptr<llvm::RuntimeDyld::MemoryManager> memManager) {
  std::vector<object::ObjectFile *>().swap(objectFiles);
  llvm::StringMap<llvm_compat::JITSymbolInfo>().swap(symbolTable);
  memoryManager = std::move(memManager);

  for (auto object : files) {
    objectFiles.push_back(object);

    for (auto symbol : object->symbols()) {
      if (symbol.getFlags() & object::SymbolRef::SF_Undefined) {
        continue;
      }

      llvm_compat::Expected<StringRef> name = symbol.getName();
      if (!name) {
        llvm_compat::ignoreError(name);
        continue;
      }

      auto flags = llvm_compat::JITSymbolFlagsFromObjectSymbol(symbol);
      symbolTable.insert(std::make_pair(name.get(), llvm_compat::JITSymbol(0, flags)));
    }

  }

  RuntimeDyld dynamicLoader(*memoryManager, resolver);
  dynamicLoader.setProcessAllSections(false);

  for (auto &object : objectFiles) {
    dynamicLoader.loadObject(*object);
  }

  for (auto &entry : symbolTable) {
    entry.second = dynamicLoader.getSymbol(entry.first());
  }

  dynamicLoader.finalizeWithMemoryManagerLocking();
}

llvm_compat::JITSymbol &JITEngine::getSymbol(llvm::StringRef name) {
  auto symbolIterator = symbolTable.find(name);
  if (symbolIterator == symbolTable.end()) {
    return symbolNotFound;
  }

  return symbolIterator->second;
}

