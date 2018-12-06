#pragma once

#include <GLES2/gl2.h>

#include "rasterizer.h"
#include "rect.h"
#include "shaper.h"
#include "splite_generator.h"

class GlyphShader {
 public:
  GlyphShader();

  void init();

  void begin();
  IRect renderGlyph(FPoint origin,
                    ISize viewport_size,
                    const Shaper::Result& glyph,
                    float textSize,
                    const Rasterizer& rasterizer);
  void finish() {}

 private:
  IRect renderGlyphInternal(FPoint origin,
                            ISize viewport_size,
                            const SpliteGenerator::Entry& entry);
  GLint shader_program_;
  GLint attr_pos_;
  GLint attr_uv_;
  GLint unif_texture_;
  GLuint texture_id_;

  SpliteGenerator splite_;
};
