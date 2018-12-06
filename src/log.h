#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GL/glu.h>

#define CHECK(cond) debug_assert(cond, #cond, __FILE__, __LINE__);
#define DCHECK(cond) debug_assert(cond, #cond, __FILE__, __LINE__);
#define GL_CHECK_NO_ERROR() debug_gl_assert(__FUNCTION__, __FILE__, __LINE__);

inline void debug_assert(bool cond,
                         const char* msg,
                         const char* file,
                         int line) {
  if (!cond) {
    fprintf(stderr, "Check failed: %s at %s:%d\n", msg, file, line);
    fflush(stderr);
    abort();
  }
}

inline void debug_gl_assert(const char* func, const char* file, int line) {
  const GLint error = glGetError();
  if (error != GL_NO_ERROR) {
    fprintf(stderr, "glGetError returns %d in %s (%s:%d)\n", error, func, file,
            line);
    fflush(stderr);
    abort();
  }
}
