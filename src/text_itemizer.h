#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include "range.h"

struct BidiResult {
  bool paraIsRtl;
  std::vector<std::pair<Range, bool>> runs;
};

BidiResult resolveBidi(const std::vector<uint32_t>& code_points,
                       Range range,
                       bool paraDir);
