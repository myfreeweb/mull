#include "JunkDetection/CXX/CXXJunkDetector.h"
#include "TestModuleFactory.h"
#include "MutationPoint.h"

#include "MutationOperators/MathAddMutationOperator.h"
#include "MutationOperators/NegateConditionMutationOperator.h"

#include "gtest/gtest.h"

#include <vector>
#include <llvm/IR/Module.h>

using namespace mull;
using namespace llvm;
using namespace std;

enum class MutationsSelector {
  AllInstructions,
  OnlyMatchingInstructions
};

vector<unique_ptr<MutationPoint>> getMutationPoints(MullModule *mullModule,
                                                    MutationOperator &mutationOperator,
                                                    MutationsSelector selector) {
  vector<unique_ptr<MutationPoint>> points;
  auto module = mullModule->getModule();

  int functionIndex = 0;
  for (auto &function : *module) {
    int blockIndex = 0;
    for (auto &basicBlock : function) {
      int instructionIndex = 0;
      for (auto &instruction : basicBlock) {
        MutationPointAddress address(functionIndex, blockIndex, instructionIndex);

        if (selector == MutationsSelector::AllInstructions) {
          points.push_back(make_unique<MutationPoint>(&mutationOperator,
                                                      address,
                                                      &instruction,
                                                      mullModule));
        } else {
          auto *point = mutationOperator.getMutationPoint(mullModule,
                                                          address,
                                                          &instruction);
          if (point) {
            points.push_back(unique_ptr<MutationPoint>(point));
          }
        }

        instructionIndex++;
      }

      blockIndex++;
    }

    functionIndex++;
  }

  return points;
}

TEST(CXXJunkDetector, math_add_c) {
  TestModuleFactory testModuleFactory;

  MathAddMutationOperator mutationOperator;
  auto mullModule = testModuleFactory.create_JunkDetection_CXX_MathC_Module();
  auto ownedPoints = getMutationPoints(mullModule.get(),
                                       mutationOperator,
                                       MutationsSelector::OnlyMatchingInstructions);

  CXXJunkDetector detector;
  vector<MutationPoint *> nonJunk;

  for (auto &point : ownedPoints) {
    MutationPoint *p = point.get();
    if (!detector.isJunk(p)) {
      nonJunk.push_back(p);
    }
  }

  ASSERT_EQ(11ul, nonJunk.size());
}

TEST(CXXJunkDetector, math_add_cpp) {
  TestModuleFactory testModuleFactory;

  MathAddMutationOperator mutationOperator;
  auto mullModule = testModuleFactory.create_JunkDetection_CXX_MathCpp_Module();
  auto ownedPoints = getMutationPoints(mullModule.get(),
                                       mutationOperator,
                                       MutationsSelector::OnlyMatchingInstructions);

  CXXJunkDetector detector;
  vector<MutationPoint *> nonJunk;

  for (auto &point : ownedPoints) {
    MutationPoint *p = point.get();
    if (!detector.isJunk(p)) {
      nonJunk.push_back(p);
    }
  }

  ASSERT_EQ(11ul, nonJunk.size());
}

TEST(CXXJunkDetector, negate_c) {
  TestModuleFactory testModuleFactory;

  NegateConditionMutationOperator mutationOperator;
  auto mullModule = testModuleFactory.create_JunkDetection_CXX_MathC_Module();
  auto ownedPoints = getMutationPoints(mullModule.get(),
                                       mutationOperator,
                                       MutationsSelector::AllInstructions);

  CXXJunkDetector detector;
  vector<MutationPoint *> nonJunk;

  for (auto &point : ownedPoints) {
    MutationPoint *p = point.get();
    if (!detector.isJunk(p)) {
      nonJunk.push_back(p);
    }
  }

  ASSERT_EQ(11ul, nonJunk.size());
}

TEST(CXXJunkDetector, negate_cpp) {
  TestModuleFactory testModuleFactory;

  NegateConditionMutationOperator mutationOperator;
  auto mullModule = testModuleFactory.create_JunkDetection_CXX_MathCpp_Module();
  auto ownedPoints = getMutationPoints(mullModule.get(),
                                       mutationOperator,
                                       MutationsSelector::AllInstructions);

  CXXJunkDetector detector;
  vector<MutationPoint *> nonJunk;

  for (auto &point : ownedPoints) {
    MutationPoint *p = point.get();
    if (!detector.isJunk(p)) {
      nonJunk.push_back(p);
    }
  }

  ASSERT_EQ(11ul, nonJunk.size());
}

