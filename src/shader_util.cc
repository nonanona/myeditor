#include "shader_util.h"
#include "log.h"

#include <memory>
#include <string>

namespace shader_util {
GLuint load(const GLchar* shader_source, const GLenum shader_type) {
  const GLint shader = glCreateShader(shader_type);
  CHECK(shader != 0);
  GL_CHECK_NO_ERROR();
  CHECK(glIsShader(shader));

  glShaderSource(shader, 1, &shader_source, nullptr);
  glCompileShader(shader);

  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    GLint info_len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
    if (info_len > 1) {
      std::unique_ptr<char> msg(new char[info_len]);
      glGetShaderInfoLog(shader, info_len, nullptr, msg.get());
      printf("%s\n", msg.get());
    } else {
      printf("No error logs\n");
    }
  }
  CHECK(success == GL_TRUE);
  return shader;
}

GLuint build(const GLchar* vertex_src, const GLchar* fragment_src) {
  const GLuint vertex_shader = load(vertex_src, GL_VERTEX_SHADER);
  const GLuint fragment_shader = load(fragment_src, GL_FRAGMENT_SHADER);

  const GLuint program = glCreateProgram();
  GL_CHECK_NO_ERROR();
  CHECK(program != 0);

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  GL_CHECK_NO_ERROR();

  glLinkProgram(program);

  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == GL_FALSE) {
    GLint info_len = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len);
    if (info_len > 1) {
      std::unique_ptr<char> msg(new char[info_len]);
      glGetProgramInfoLog(program, info_len, nullptr, msg.get());
      printf("%s\n", msg.get());
    } else {
      printf("No error logs\n");
    }
  }
  CHECK(success == GL_TRUE);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return program;
}

}  // namespace shader_util
