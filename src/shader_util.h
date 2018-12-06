#pragma once

#include <GLES2/gl2.h>

namespace shader_util {

GLuint build(const GLchar* vertex_src, const GLchar* fragment_src);

}  // namespace shader_util
