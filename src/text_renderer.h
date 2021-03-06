#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "glyph_shader.h"
#include "line_breaker.h"
#include "rasterizer.h"
#include "rect.h"
#include "rect_shader.h"
#include "shaper.h"
#include "selection.h"
#include "view.h"

class TextRenderer {
 public:
  TextRenderer(EGLDisplay display,
               EGLContext context,
               EGLSurface surface,
               IRect target,
               Border border,
               Padding padding)
      : display_(display),
        context_(context),
        surface_(surface),
        target_rect_(target),
        border_(border),
        padding_(padding) {}
  ~TextRenderer() {}

  void init(float textSizePx);
  void renderText(const Text& text, const Selection& selection);
  void onCursorDraw();

 private:
  void renderTextInternal();
  void drawHelperLines();
  void drawHorizontalLines(float base_line, float line_top, float line_bottom);
  IRect getCursorRect(uint32_t offset);
  std::vector<IRect> getSelectionBoxes(uint32_t start, uint32_t end);

  EGLDisplay display_;
  EGLContext context_;
  EGLSurface surface_;

  IRect target_rect_;
  Border border_;
  Padding padding_;

  IRect drawing_area_;

  Paint paint_;

  Shaper shaper_;
  GlyphShader glyph_shader_;
  RectShader rect_shader_;
  Rasterizer rasterizer_;

  float text_size_px_;

  // last drawn text
  Text text_;
  std::vector<line_breaker::Line> last_shape_result_;

  // Copy of selection
  Selection selection_;
  std::vector<IRect> selection_boxes_;
  IRect cursor_rect_;
  bool cursor_visible_ = false;
};
