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

  class NegateConditionMutator : public Mutator {
    
  public:
    static const std::string ID;

    static llvm::CmpInst::Predicate negatedCmpInstPredicate(llvm::CmpInst::Predicate predicate);

    MutationPoint *getMutationPoint(MullModule *module,
                                        MutationPointAddress &address,
                                        llvm::Instruction *instruction,
                                        SourceLocation &sourceLocation) override;
    MutatorKind mutatorKind() override { return MutatorKind::NegateMutator; }
    std::string getUniqueIdentifier() override {
      return ID;
    }
    std::string getUniqueIdentifier() const override {
      return ID;
    }

    bool canBeApplied(llvm::Value &V) override;
    llvm::Value *
    applyMutation(llvm::Module *module, MutationPointAddress &address) override;
  };
}
