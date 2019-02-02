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
      Line emptyLine;
      emptyLine.isRtl = false;
      emptyLine.startOffset = 0;
      FontFallback::Font::Metrics metrics =
          shaper.defaultFont().metrics(shaper.textSize());
      emptyLine.ascent = metrics.ascender;
      emptyLine.descent = metrics.descender;
      result.push_back(Line());
      continue;
    }

    float remainingWidth = width;
    Line currentLine;
    auto [glyphs, isParaRtl] =
        shaper.shape(code_points, paraRange, rasterizer, false);

    currentLine.isRtl = isParaRtl;
    currentLine.startOffset = paraRange.start();
    currentLine.ascent = 0;
    currentLine.descent = 0;
    for (const Shaper::Result& r : glyphs) {
      if (remainingWidth < r.x_advance_) {
        // No more characters. break line
        currentLine.endOffset = r.cluster_idx_;
        result.push_back(currentLine);

        // Prepare next line
        currentLine = Line();
        currentLine.isRtl = isParaRtl;
        currentLine.startOffset = r.cluster_idx_;
        remainingWidth = width;
      }
      currentLine.glyphs.push_back(r);

      FontFallback::Font::Metrics metrics = r.font_.metrics(shaper.textSize());
      currentLine.ascent = std::max(metrics.ascender, currentLine.ascent);
      currentLine.descent = std::min(metrics.descender, currentLine.descent);
      remainingWidth -= r.x_advance_;
    }
    currentLine.endOffset = paraRange.end();
    result.push_back(currentLine);
  }

  return result;
}

}  // namespace line_breaker
