#pragma once

#include "JunkDetection/JunkDetector.h"

#include <map>
#include <string>
#include <clang-c/Index.h>

namespace mull {

class MutationPoint;

struct PhysicalAddress {
  std::string filepath;
  uint32_t line;
  uint32_t column;
  PhysicalAddress() : filepath(""), line(0), column(0) {}
  bool valid() {
    if (filepath != "" && line != 0 && column != 0) {
      return true;
    }

    return false;
  }
};

class CXXJunkDetector : public JunkDetector {
public:
  CXXJunkDetector();
  ~CXXJunkDetector();

  bool isJunk(MutationPoint *point) override;
private:
  std::pair<CXCursor, CXSourceLocation> cursorAndLocation(PhysicalAddress &address);

  CXIndex index;
  std::map<std::string, CXTranslationUnit> units;
};

}
