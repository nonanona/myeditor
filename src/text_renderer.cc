#include "text_renderer.h"

#include "line_breaker.h"
#include "option_parser.h"

#include <math.h>

std::pair<float, float> TextRenderer::metricsForLine(
    const line_breaker::Line& line) {
  if (line.glyphs.empty()) {
    FontFallback::Font::Metrics metrics =
        shaper_.defaultFont().metrics(text_size_px_);
    return std::make_pair(metrics.ascender, metrics.descender);
  } else {
    float ascent = 0;
    float descent = 0;
    for (auto r : line.glyphs) {
      FontFallback::Font::Metrics metrics = r.font_.metrics(text_size_px_);
      ascent = std::max(metrics.ascender, ascent);
      descent = std::min(metrics.descender, descent);
    }
    return std::make_pair(ascent, descent);
  }
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

void TextRenderer::renderText(const Text& text, uint32_t cursor_pos) {
  text_ = text;
  last_shape_result_ = line_breaker::breakToLines(
      shaper_, text, drawing_area_.width(), rasterizer_);
  if (Option::get().isVerboseMode()) {
    for (auto line : last_shape_result_) {
      printf("Line 0: \n");
      for (auto glyph : line.glyphs) {
        printf("  %s\n", glyph.toString().c_str());
      }
    }
  }
  cursor_offset_ = cursor_pos;
  renderTextInternal();
}

void TextRenderer::renderTextInternal() {
  eglMakeCurrent(display_, surface_, surface_, context_);
  glViewport(drawing_area_.l(), drawing_area_.t(), drawing_area_.width(),
             drawing_area_.height());

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  uint32_t lineCount = last_shape_result_.size();

  bool cursor_drawn = false;
  uint32_t idx = 0;

  float x_advance = 0.0f;
  float lineTop = 0;
  for (uint32_t i = 0; i < lineCount; ++i) {
    const line_breaker::Line& line = last_shape_result_[i];

    auto [ascent, descent] = metricsForLine(line);
    float lineHeight = ascent - descent;
    float drawOriginY = lineTop + ascent;

    x_advance = 0.0f;
    if (line.isRtl) {
      for (auto r : line.glyphs) {
        x_advance += r.x_advance_;
      }
      x_advance = drawing_area_.width() - x_advance;
    }
    for (auto r : line.glyphs) {
      glyph_shader_.begin();
      IRect rect = glyph_shader_.renderGlyph(FPoint(x_advance, drawOriginY),
                                             drawing_area_.size(), r,
                                             text_size_px_, rasterizer_);
      glyph_shader_.finish();

      if (Option::get().isDebugMode()) {
        rect_shader_.begin(drawing_area_.size());
        rect_shader_.renderGlyphRect(FPoint(x_advance, drawOriginY), rect, r);
        rect_shader_.finish();
      }

      if (cursor_offset_ == idx) {
        cursor_drawn = true;
        cursor_rect_.set(IPoint(x_advance, lineHeight * i),
                         ISize(2, ascent - descent));
      }
      idx++;
      x_advance += r.x_advance_;
    }
    lineTop += lineHeight;
  }

  if (!cursor_drawn) {
    float ascent = 0;
    float descent = 0;
    if (last_shape_result_.empty()) {
      FontFallback::Font::Metrics metrics =
          shaper_.defaultFont().metrics(text_size_px_);
      ascent = metrics.ascender;
      descent = metrics.descender;
    } else {
      std::tie(ascent, descent) = metricsForLine(last_shape_result_.back());
    }
    float lineHeight = ascent - descent;
    cursor_rect_.set(IPoint(x_advance, lineHeight * (lineCount - 1)),
                     ISize(2, ascent - descent));
  }

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
    FontFallback::Font::Metrics metrics;
    auto [ascent, descent] = metricsForLine(last_shape_result_[i]);
    float lineHeight = ascent - descent;
    float baseLineY = lineTop + ascent;
    float lineBottom = baseLineY - descent;
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
