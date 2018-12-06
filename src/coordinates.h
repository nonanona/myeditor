#pragma once

#include "rect.h"

inline float scrToNDFX(float x, float viewport_width) {
  return ((2.0f * x) / viewport_width) - 1.0f;
}

inline float scrToNDFX(float x, ISize viewport_size) {
  return scrToNDFX(x, (float)viewport_size.width());
}

inline float scrToNDFY(float y, float viewport_height) {
  return -1.0f * (((2.0f * y) / viewport_height) - 1.0f);
}

inline float scrToNDFY(float y, ISize viewport_size) {
  return scrToNDFY(y, (float)viewport_size.height());
}

class ScreenToNDFCoordConverter {
 public:
  ScreenToNDFCoordConverter(ISize viewport_size)
      : width_(viewport_size.width()), height_(viewport_size.height()) {}

  inline float toX(float x) const { return scrToNDFX(x, width_); }
  inline float toY(float y) const { return scrToNDFY(y, height_); }

 private:
  float width_;
  float height_;
};
