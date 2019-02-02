#include "glyph_shader.h"

#include "coordinates.h"
#include "log.h"
#include "shader_util.h"

#include <memory>
#include <string>

GlyphShader::GlyphShader() {}

void GlyphShader::init() {
  const GLchar* vertex_shader_source =
      "attribute vec4 attr_pos;"
      "attribute vec2 attr_uv;"
      "varying mediump vec2 vary_uv;"
      "void main() {"
      "  gl_Position = attr_pos;"
      "  vary_uv = attr_uv;"
      "}";
  const GLchar* fragment_shader_source =
      "uniform sampler2D texture;"
      "varying mediump vec2 vary_uv;"
      "void main() {"
      "    mediump vec4 colour = texture2D(texture, vary_uv);"
      "    if (colour.r > 0.1) {"
      "      gl_FragColor = 1.0 - colour;"
      "    } else {"
      "      discard;"
      "    }"
      "}";

  shader_program_ =
      shader_util::build(vertex_shader_source, fragment_shader_source);

  attr_pos_ = glGetAttribLocation(shader_program_, "attr_pos");
  CHECK(attr_pos_ >= 0);

  attr_uv_ = glGetAttribLocation(shader_program_, "attr_uv");
  CHECK(attr_uv_ >= 0);

  unif_texture_ = glGetUniformLocation(shader_program_, "texture");
  CHECK(unif_texture_ >= 0);

  glGenTextures(1, &texture_id_);
  CHECK(texture_id_ != 0);
  GL_CHECK_NO_ERROR();

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GL_CHECK_NO_ERROR();

  glBindTexture(GL_TEXTURE_2D, texture_id_);
  GL_CHECK_NO_ERROR();

  ISize size = splite_.capacity();
  std::vector<uint8_t> tmp;
  tmp.resize(size.size());

  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, size.width(), size.height(), 0,
               GL_LUMINANCE, GL_UNSIGNED_BYTE, tmp.data());
  GL_CHECK_NO_ERROR();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GL_CHECK_NO_ERROR();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  GL_CHECK_NO_ERROR();
}

void GlyphShader::begin() {
  glUseProgram(shader_program_);
  GL_CHECK_NO_ERROR();

  glEnableVertexAttribArray(attr_pos_);
  GL_CHECK_NO_ERROR();

  glEnableVertexAttribArray(attr_uv_);
  GL_CHECK_NO_ERROR();

  glUniform1i(unif_texture_, 0);
  GL_CHECK_NO_ERROR();
}

IRect GlyphShader::renderGlyph(FPoint origin,
                               ISize viewport_size,
                               const Shaper::Result& r,
                               float textSize,
                               const Rasterizer& rasterizer) {
  SpliteGenerator::Key key(r.font_, r.glyph_id_, textSize);
  std::optional<SpliteGenerator::Entry> cached_entry = splite_.lookup(key);

  if (cached_entry.has_value()) {
    return renderGlyphInternal(origin, viewport_size, *cached_entry);
  } else {
    A8Bitmap bmp = rasterizer.rasterize(r.font_, r.glyph_id_, textSize);
    const SpliteGenerator::Entry& entry = splite_.store(key, bmp.size(), r);

    glTexSubImage2D(GL_TEXTURE_2D, 0, entry.rect.l(), entry.rect.t(),
                    entry.rect.width(), entry.rect.height(), GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, bmp.buffer());
    GL_CHECK_NO_ERROR();

    return renderGlyphInternal(origin, viewport_size, entry);
  }
}

IRect GlyphShader::renderGlyphInternal(FPoint origin,
                                       ISize viewport_size,
                                       const SpliteGenerator::Entry& entry) {
  const Shaper::Result& g = entry.glyph;
  const IRect& rec = entry.rect;
  const FRect& uv_rec = entry.uv_rect;

  float origin_x = origin.x() + g.x_offset_;
  float origin_y = origin.y() + g.y_offset_;

  float bmp_x = origin_x + g.x_bearing_;
  float bmp_y = origin_y - g.y_bearing_;

  ScreenToNDFCoordConverter c(viewport_size);

  const GLfloat position[] = {
      c.toX(bmp_x),
      c.toY(bmp_y),
      c.toX(bmp_x),
      c.toY(bmp_y + rec.height()),
      c.toX(bmp_x + rec.width()),
      c.toY(bmp_y),
      c.toX(bmp_x + rec.width()),
      c.toY(bmp_y + rec.height()),
  };

  const GLfloat uv[]{
      uv_rec.l(), uv_rec.t(), uv_rec.l(), uv_rec.b(),
      uv_rec.r(), uv_rec.t(), uv_rec.r(), uv_rec.b(),
  };

  glVertexAttribPointer(attr_pos_, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)position);
  GL_CHECK_NO_ERROR();
  glVertexAttribPointer(attr_uv_, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)uv);
  GL_CHECK_NO_ERROR();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  GL_CHECK_NO_ERROR();

  return entry.rect;
}
