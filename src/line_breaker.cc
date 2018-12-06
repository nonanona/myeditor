#include "line_breaker.h"

namespace line_breaker {

std::vector<Range> split(const std::vector<uint32_t>& code_points,
                         uint32_t delim) {
  std::vector<Range> res;
  uint32_t prev = 0;
  uint32_t i = 0;
  for (; i < code_points.size(); ++i) {
    if (code_points[i] == delim) {
      res.push_back(Range(prev, i));
      prev = i + 1;
    }
  }
  if (prev != code_points.size()) {
    if (code_points.back() == delim) {
      res.push_back(Range(prev, code_points.size() - 1));
    } else {
      res.push_back(Range(prev, code_points.size()));
    }
  }
  return res;
}

std::vector<Line> breakToLines(const Shaper& shaper,
                               const Text& text,
                               uint32_t width,
                               const Rasterizer& rasterizer) {
  // deperate breaking. Assuming at least one character can fit in a line.
  std::vector<Line> result;

  const std::vector<uint32_t>& code_points = text.code_points();

  for (Range paraRange : split(code_points, '\n')) {
    if (paraRange.empty()) {
      result.push_back(Line());
      continue;
    }

    float remainingWidth = width;
    Line currentLine;
    auto [glyphs, isParaRtl] =
        shaper.shape(code_points, paraRange, rasterizer, false);

    currentLine.isRtl = isParaRtl;
    for (const Shaper::Result& r : glyphs) {
      if (remainingWidth < r.x_advance_) {
        // No more characters. break line
        result.push_back(currentLine);
        currentLine = Line();
        currentLine.isRtl = isParaRtl;
        remainingWidth = width;
      }
      currentLine.glyphs.push_back(r);
      remainingWidth -= r.x_advance_;
    }
    result.push_back(currentLine);
  }
  return result;
}

}  // namespace line_breaker
