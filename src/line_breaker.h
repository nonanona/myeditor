#pragma once

#include <vector>

#include "rasterizer.h"
#include "shaper.h"
#include "text.h"

namespace line_breaker {

struct Line {
  bool isRtl;
  uint32_t startOffset;
  uint32_t endOffset;
  float ascent;
  float descent;
  std::vector<Shaper::Result> glyphs;

  float getWidth() const {  // cache?
    float x_advance = 0;
    for (const auto& r : glyphs) {
      x_advance += r.x_advance_;
    }
    return x_advance;
  }

  float getHeight() const {
      return ascent - descent;
  }
};

std::vector<Line> breakToLines(const Shaper& shaper,
                               const Text& text,
                               uint32_t width,
                               const Rasterizer& rasterizer);

}  // namespace line_breaker
