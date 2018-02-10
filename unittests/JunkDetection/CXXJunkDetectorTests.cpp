#include "JunkDetection/CXX/CXXJunkDetector.h"
#include "TestModuleFactory.h"
#include "MutationOperators/MathAddMutationOperator.h"
#include "MutationPoint.h"

#include "gtest/gtest.h"

#include <vector>
#include <llvm/IR/Module.h>

using namespace mull;
using namespace llvm;
using namespace std;

vector<unique_ptr<MutationPoint>> getMutationPoints(MullModule *mullModule,
                                                    MutationOperator *mutationOperator) {
  vector<unique_ptr<MutationPoint>> points;
  auto module = mullModule->getModule();

  int functionIndex = 0;
  for (auto &function : *module) {
    int blockIndex = 0;
    for (auto &basicBlock : function) {
      int instructionIndex = 0;
      for (auto &instruction : basicBlock) {
        MutationPointAddress address(functionIndex, blockIndex, instructionIndex);

        points.push_back(make_unique<MutationPoint>(mutationOperator,
                                                    address,
                                                    &instruction,
                                                    mullModule));

        instructionIndex++;
      }

      blockIndex++;
    }

    functionIndex++;
  }

  return points;
}

TEST(CXXJunkDetector, math_c) {
  TestModuleFactory testModuleFactory;

  MathAddMutationOperator mutationOperator;
  auto mullModule = testModuleFactory.create_JunkDetection_CXX_MathC_Module();
  auto ownedPoints = getMutationPoints(mullModule.get(), &mutationOperator);

  CXXJunkDetector detector;
  vector<MutationPoint *> nonJunk;
  for (auto &point : ownedPoints) {
    MutationPoint *p = point.get();
    if (detector.isJunk(p)) {
      nonJunk.push_back(p);
    }
  }

  ASSERT_EQ(0ul, nonJunk.size());
}
