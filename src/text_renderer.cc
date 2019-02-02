#include "text_renderer.h"

#include "line_breaker.h"
#include "option_parser.h"
#include "text_itemizer.h"

#include <math.h>
#include <functional>


using LineCallback = std::function<void(
    const line_breaker::Line& line,
    uint32_t lineNo,
    uint32_t lineCount,
    float lineTop)>;

using GlyphCallback = std::function<void(
    const line_breaker::Line& line,
    const Shaper::Result& glyph,
    uint32_t lineNo,
    uint32_t lineCount,
    float lineTop,
    float glyphLeft)>;

float getDrawLeft(const line_breaker::Line& line, float width) {
  if (line.isRtl) {
    return width - line.getWidth();
  } else {
    return 0.0f;
  }
}

float getDrawRight(const line_breaker::Line& line, float width) {
  if (line.isRtl) {
    return width;
  } else {
    return line.getWidth();
  }
}

void processLines(std::vector<line_breaker::Line>& lines,
                  const LineCallback& callback) {
  uint32_t lineCount = lines.size();
  float lineTop = 0;
  for (uint32_t i = 0; i < lineCount; ++i) {
    const line_breaker::Line& line = lines[i];
    callback(line, i, lineCount, lineTop);
    lineTop += line.ascent - line.descent;
  }
}

void processSingleLine(const line_breaker::Line& line, float width, uint32_t lineNo,
                       uint32_t lineCount, float lineTop, const GlyphCallback& callback) {
  float x_advance = getDrawLeft(line, width);
  for (const auto& r : line.glyphs) {
    callback(line, r, lineNo, lineCount, lineTop, x_advance);
    x_advance += r.x_advance_;
  }
}

void processGlyph(std::vector<line_breaker::Line>& lines, float width,
                  const GlyphCallback& callback) {
    processLines(lines, [=](
                      const line_breaker::Line& line,
                      uint32_t lineNo,
                      uint32_t lineCount,
                      float lineTop) {
        processSingleLine(line, width, lineNo, lineCount, lineTop, callback);
    });
}

void TextRenderer::init(float text_size_px) {
  text_size_px_ = text_size_px;
  shaper_.init(text_size_px_);

  drawing_area_ = IRect(IPoint(border_.l, border_.t),
                        ISize(target_rect_.width() - border_.l - border_.r,
                              target_rect_.height() - border_.t - border_.b));
  glyph_shader_.init();
  rect_shader_.init();
}

void TextRenderer::renderText(const Text& text, const Selection& selection) {
  text_ = text;
  last_shape_result_ = line_breaker::breakToLines(
      shaper_, text, drawing_area_.width(), rasterizer_);
  selection_ = selection;
  renderTextInternal();
}

IRect computeBBox(const line_breaker::Line& line, float width, uint32_t lineNo, uint32_t lineCount, float lineTop,
                  const Range& range) {
  std::unique_ptr<IRect> rect;
  processSingleLine(line, width, lineNo, lineCount, lineTop,
      [&](const line_breaker::Line& line, const Shaper::Result& glyph, uint32_t lineNo, uint32_t lineCount,
          float lineTop, float glyphLeft) {
    if (range.contains(glyph.cluster_idx_)) {
      if (!rect) {
        rect = std::make_unique<IRect>(IRect::createLTWH(glyphLeft, lineTop, glyph.x_advance_, line.getHeight()));
      } else {
        rect->unionWith(IRect::createLTWH(glyphLeft, lineTop, glyph.x_advance_, line.getHeight()));
      }
    }
  });
  CHECK(rect.get() != nullptr);
  return *rect;
}

std::vector<IRect> collectBoxesInLine(const Text& text, const line_breaker::Line& line, float width,
                                      uint32_t lineNo, uint32_t lineCount, float lineTop, uint32_t start,
                                      uint32_t end) {
  std::vector<IRect> result;
  if (end < line.startOffset || line.endOffset < start) {
    return result;  // No selection in this line.
  }

  if (start <= line.startOffset && line.endOffset <= end) {
    // Whole line is selected.
    result.push_back(IRect::createLTWH(
        getDrawLeft(line, width),
        lineTop,
        line.getWidth(),
        line.getHeight()));
    return result;
  }

  // Selection anchor is in the line. Adjuest the start/end to line start/end.
  start = std::max(line.startOffset, start);
  end = std::min(line.endOffset, end);

  BidiResult bidi = resolveBidi(text.code_points(), Range(start, end), line.isRtl);

  Range range(start, end);

  for (auto[runRange, rtl] : bidi.runs) {
    if (!range.intersect(runRange)) {
      continue;  // No intersection with this run. skipping.
    }
    const auto i = range.intersection(runRange);
    result.push_back(computeBBox(line, width, lineNo, lineCount, lineTop, range.intersection(runRange)));
  }

  return result;
}

std::vector<IRect> TextRenderer::getSelectionBoxes(uint32_t start, uint32_t end) {
  std::vector<IRect> res;
  processLines(last_shape_result_,
      [&](const line_breaker::Line& line, uint32_t lineNo, uint32_t lineCount, float lineTop) {
    std::vector<IRect> result = collectBoxesInLine(text_, line, drawing_area_.width(), lineNo, lineCount,
                                                   lineTop, start, end);
    res.insert(res.end(), result.begin(), result.end());
  });
  return res;
}

IRect TextRenderer::getCursorRect(uint32_t offset) {
  IRect result;
  processLines(last_shape_result_,
      [&](const line_breaker::Line& line, uint32_t lineNo, uint32_t lineCount, float lineTop) {
    const bool isLastLine = (lineNo == (lineCount - 1));
    const float lineHeight = line.ascent - line.descent;

    float cursor_left = 0.0f;
    if (line.startOffset <= offset && offset < line.endOffset) { // The cursor is middle of this line
      uint32_t bestClusterIndex = 0;
      processSingleLine(line, drawing_area_.width(), lineNo, lineCount, lineTop,
          [&](const line_breaker::Line& line, const Shaper::Result& glyph, uint32_t lineNo, uint32_t lineCount,
              float linetop, float glyphLeft) {
          if (bestClusterIndex < glyph.cluster_idx_ && glyph.cluster_idx_ <= offset) {
            bestClusterIndex = glyph.cluster_idx_;
            cursor_left = glyphLeft;
          }
      });
    } else if (isLastLine && offset == line.endOffset) {
      // If the cursor is at the end of line, draw cursor if this line is the last line.
      if (line.isRtl) {
        cursor_left = getDrawLeft(line, drawing_area_.width());
      } else {
        cursor_left = getDrawRight(line, drawing_area_.width());
      }
    } else {
      return;  // cursor is not in this line.
    }
    result = IRect(IPoint(cursor_left, lineHeight * lineNo), ISize(2, lineHeight));
  });
  return result;
}

void TextRenderer::renderTextInternal() {
  eglMakeCurrent(display_, surface_, surface_, context_);
  glViewport(drawing_area_.l(), drawing_area_.t(), drawing_area_.width(),
             drawing_area_.height());

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw Selection
  if (!selection_.isCollapsed()) {
    selection_boxes_ = getSelectionBoxes(selection_.start(), selection_.end());
    rect_shader_.begin(drawing_area_.size());
    for (const auto& rect : selection_boxes_) {
      rect_shader_.fillRect(rect, 0xFFC0C0FF);
    }
    rect_shader_.finish();
  }

  // Draw glyphs
  processGlyph(last_shape_result_, drawing_area_.width(),
       [=](const line_breaker::Line& line, const Shaper::Result& glyph, uint32_t lineNo, uint32_t lineCount,
           float lineTop, float glyphLeft) {
    const float drawOriginY = lineTop + line.ascent;

    glyph_shader_.begin();
    IRect rect = glyph_shader_.renderGlyph(FPoint(glyphLeft, drawOriginY),
                                           drawing_area_.size(), glyph,
                                           text_size_px_, rasterizer_);
    glyph_shader_.finish();

    if (Option::get().isDebugMode()) {
      rect_shader_.begin(drawing_area_.size());
      rect_shader_.renderGlyphRect(FPoint(glyphLeft, drawOriginY), rect, glyph);
      rect_shader_.finish();
    }
  });


  // Draw Cursor
  cursor_rect_ = getCursorRect(selection_.start());
  if (cursor_visible_) {
    rect_shader_.begin(drawing_area_.size());
    rect_shader_.fillRect(cursor_rect_, 0x000000FF);
    rect_shader_.finish();
  }

  if (Option::get().isDebugMode()) {
    drawHelperLines();
  }
  eglSwapBuffers(display_, surface_);
}

void TextRenderer::drawHelperLines() {
  uint32_t lineCount = last_shape_result_.size();
  if (lineCount == 0) {
    return;
  }
  float lineTop = 0;
  for (uint32_t i = 0; i < lineCount; ++i) {
    const line_breaker::Line& line = last_shape_result_[i];
    float lineHeight = line.ascent - line.descent;
    float baseLineY = lineTop + line.ascent;
    float lineBottom = baseLineY - line.descent;
    drawHorizontalLines(baseLineY, lineTop, lineBottom);
    lineTop += lineHeight;
  }
}

void TextRenderer::drawHorizontalLines(float base_line,
                                       float line_top,
                                       float line_bottom) {
  rect_shader_.begin(drawing_area_.size());
  rect_shader_.fillRect(
      IRect(IPoint(0, base_line), ISize(drawing_area_.width(), 2)), 0xFF0000FF);
  rect_shader_.fillRect(
      IRect(IPoint(0, line_top), ISize(drawing_area_.width(), 2)), 0x0000FFFF);
  rect_shader_.fillRect(
      IRect(IPoint(0, line_bottom), ISize(drawing_area_.width(), 2)),
      0x0000FFFF);
  rect_shader_.finish();
}

void TextRenderer::onCursorDraw() {
  cursor_visible_ = !cursor_visible_;
  renderTextInternal();
}
