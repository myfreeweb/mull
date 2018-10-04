#pragma once

#include "Mutators/Mutator.h"

#include <vector>

namespace mull {

class MutationPoint;
class MutationPointAddress;

class ReplaceAssignmentMutator : public Mutator {

public:
  static const std::string ID;

  MutationPoint *getMutationPoint(MullModule *module,
                                    llvm::Function *function,
                                    llvm::Instruction *instruction,
                                    SourceLocation &sourceLocation,
                                    MutationPointAddress &address) override;

  std::string getUniqueIdentifier() override {
    return ID;
  }
  std::string getUniqueIdentifier() const override {
    return ID;
  }

  bool canBeApplied(llvm::Value &V) override;
  llvm::Value *
  applyMutation(llvm::Function *function, MutationPointAddress &address) override;
};

}
