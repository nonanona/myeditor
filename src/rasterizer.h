#pragma once

#include <hb.h>
#include <stdint.h>
#include <string>
#include <vector>

#include <unordered_map>

#include "font_fallback.h"
#include "ft_util.h"
#include "macro.h"
#include "rect.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class A8Bitmap {
 public:
  A8Bitmap(int w, int h) : w_(w), h_(h) {
    row_bytes_ = ((w + 3) / 4) * 4;
    buffer_.resize(row_bytes_ * h);
  }

  uint8_t* buffer() { return buffer_.data(); }
  const uint8_t* buffer() const { return buffer_.data(); }

  int w() const { return w_; }
  int h() const { return h_; }
  int rowBytes() const { return w_; }
  ISize size() const { return ISize(w_, h_); }

  A8Bitmap(A8Bitmap&&) = default;
  A8Bitmap& operator=(A8Bitmap&&) = default;

  void saveToFile(const std::string& file) const;

 private:
  int w_;
  int row_bytes_;
  int h_;
  std::vector<uint8_t> buffer_;

  DISALLOW_COPY_AND_ASSIGN(A8Bitmap);
};

class Rasterizer {
 public:
  Rasterizer();
  ~Rasterizer() {}

  A8Bitmap rasterize(const FontFallback::Font& font,
                     uint32_t glyph_id,
                     float size) const;

  IRect bbox(const FontFallback::Font& font,
             uint32_t glyph_id,
             float size) const;

 private:
  FT_Library library_;
};
