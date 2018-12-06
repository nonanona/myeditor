#include "rect_shader.h"

#include "coordinates.h"
#include "log.h"
#include "shader_util.h"

#include <memory>
#include <string>

RectShader::RectShader() {}

void RectShader::init() {
  const GLchar* vertex_shader_source =
      "attribute vec4 attr_pos;"
      "void main() {"
      "  gl_Position = attr_pos;"
      "}";
  const GLchar* fragment_shader_source =
      "uniform lowp vec4 unif_color;"
      "void main() {"
      "    gl_FragColor = unif_color;"
      "}";

  shader_program_ =
      shader_util::build(vertex_shader_source, fragment_shader_source);

  attr_pos_ = glGetAttribLocation(shader_program_, "attr_pos");
  CHECK(attr_pos_ >= 0);

  unif_color_ = glGetUniformLocation(shader_program_, "unif_color");
  CHECK(unif_color_ >= 0);
}

void RectShader::begin(ISize viewport_size) {
  viewport_size_ = viewport_size;
  glUseProgram(shader_program_);
  GL_CHECK_NO_ERROR();

  glEnableVertexAttribArray(attr_pos_);
  GL_CHECK_NO_ERROR();
}

void RectShader::renderGlyphRect(FPoint origin,
                                 IRect bmp_rect,
                                 const Shaper::Result& r) {
  glLineWidth(1.0f);
  float origin_x = origin.x() + r.x_offset_;
  float origin_y = origin.y() + r.y_offset_;

  float bmp_x = origin_x + r.x_bearing_;
  float bmp_y = origin_y - r.y_bearing_;

  ScreenToNDFCoordConverter c(viewport_size_);

  const GLfloat position[] = {
      c.toX(bmp_x),
      c.toY(bmp_y),
      c.toX(bmp_x),
      c.toY(bmp_y + bmp_rect.height()),
      c.toX(bmp_x + bmp_rect.width()),
      c.toY(bmp_y + bmp_rect.height()),
      c.toX(bmp_x + bmp_rect.width()),
      c.toY(bmp_y),
  };

  const GLfloat color[] = {0.0f, 0.0f, 1.0f, 1.0f};
  glUniform4fv(unif_color_, 1, color);
  GL_CHECK_NO_ERROR();

  glVertexAttribPointer(attr_pos_, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)position);
  GL_CHECK_NO_ERROR();
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  GL_CHECK_NO_ERROR();
}

void RectShader::renderRect(IRect rect, float lineWidth, uint32_t argb) const {
  glLineWidth(lineWidth);

  const GLfloat color[] = {(float)((argb >> 24) & 0xFF) / 255.0f,
                           (float)((argb >> 16) & 0xFF) / 255.0f,
                           (float)((argb >> 8) & 0xFF) / 255.0f,
                           (float)((argb)&0xFF) / 255.0f};

  glUniform4fv(unif_color_, 1, color);
  GL_CHECK_NO_ERROR();

  ScreenToNDFCoordConverter c(viewport_size_);
  const GLfloat position[] = {
      c.toX(rect.l()), c.toY(rect.t()), c.toX(rect.l()), c.toY(rect.b()),
      c.toX(rect.r()), c.toY(rect.b()), c.toX(rect.r()), c.toY(rect.t()),
  };

  glVertexAttribPointer(attr_pos_, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)position);
  GL_CHECK_NO_ERROR();
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  GL_CHECK_NO_ERROR();
}

void RectShader::fillRect(IRect rect, uint32_t argb) const {
  const GLfloat color[] = {(float)((argb >> 24) & 0xFF) / 255.0f,
                           (float)((argb >> 16) & 0xFF) / 255.0f,
                           (float)((argb >> 8) & 0xFF) / 255.0f,
                           (float)((argb)&0xFF) / 255.0f};

  glUniform4fv(unif_color_, 1, color);
  GL_CHECK_NO_ERROR();

  ScreenToNDFCoordConverter c(viewport_size_);
  const GLfloat position[] = {
      c.toX(rect.l()), c.toY(rect.t()), c.toX(rect.l()), c.toY(rect.b()),
      c.toX(rect.r()), c.toY(rect.t()), c.toX(rect.r()), c.toY(rect.b()),
  };

  glVertexAttribPointer(attr_pos_, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)position);
  GL_CHECK_NO_ERROR();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  GL_CHECK_NO_ERROR();
}
