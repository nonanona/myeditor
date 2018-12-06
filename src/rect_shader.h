#pragma once

#include <GLES2/gl2.h>

#include "rasterizer.h"
#include "rect.h"
#include "shaper.h"
#include "splite_generator.h"

class RectShader {
 public:
  RectShader();

  void init();

  void begin(ISize viewport_size);
  void renderGlyphRect(FPoint origin,
                       IRect bmp_rect,
                       const Shaper::Result& glyph);
  void renderRect(IRect rect, float lineWidth, uint32_t argb) const;
  void fillRect(IRect rect, uint32_t argb) const;
  void finish() {}

 private:
  GLint shader_program_;
  GLint unif_color_;
  GLint attr_pos_;

  ISize viewport_size_;
};
