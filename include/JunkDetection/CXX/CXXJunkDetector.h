#pragma once

#include "JunkDetection/JunkDetector.h"

namespace mull {

class MutationPoint;

class CXXJunkDetector : public JunkDetector {
public:
  bool isJunk(MutationPoint *point) override;
};

}
