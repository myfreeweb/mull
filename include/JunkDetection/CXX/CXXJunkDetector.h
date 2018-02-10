#pragma once

#include "JunkDetection/JunkDetector.h"

#include <map>
#include <string>
#include <clang-c/Index.h>

namespace mull {

class MutationPoint;

class CXXJunkDetector : public JunkDetector {
public:
  CXXJunkDetector();
  ~CXXJunkDetector();

  bool isJunk(MutationPoint *point) override;
private:
  CXIndex index;
  std::map<std::string, CXTranslationUnit> units;
};

}
