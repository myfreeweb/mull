#pragma once

#include "Mutators/Mutator.h"

#include <llvm/IR/Instructions.h>

#include <vector>

namespace llvm {
class Instruction;
}

namespace mull {

class MullModule;
class MutationPoint;
class MutationPointAddress;

class ConditionalsBoundaryMutator : public Mutator {

public:
  static const std::string ID;
  std::string getUniqueIdentifier() override;
  std::string getUniqueIdentifier() const override;
  MutatorKind mutatorKind() override;

  static bool isGT(llvm::Instruction *instruction);
  static bool isGTE(llvm::Instruction *instruction);
  static bool isLT(llvm::Instruction *instruction);
  static bool isLTE(llvm::Instruction *instruction);

  MutationPoint *getMutationPoint(MullModule *module,
                                    MutationPointAddress &address,
                                    llvm::Instruction *instruction,
                                    SourceLocation &sourceLocation) override;

  bool canBeApplied(llvm::Value &V) override;
  llvm::Value *
  applyMutation(llvm::Module *module, MutationPointAddress &address) override;
};

}

