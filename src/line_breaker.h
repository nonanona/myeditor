#pragma once

#include <vector>

#include "rasterizer.h"
#include "shaper.h"
#include "text.h"

namespace line_breaker {

struct Line {
  bool isRtl;
  std::vector<Shaper::Result> glyphs;
};

std::vector<Line> breakToLines(const Shaper& shaper,
                               const Text& text,
                               uint32_t width,
                               const Rasterizer& rasterizer);

}  // namespace line_breaker
