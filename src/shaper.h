#pragma once

#include <string>
#include <vector>

#include <hb.h>

#include "font_fallback.h"
#include "hb_util.h"
#include "rasterizer.h"
#include "text.h"

struct Paint {
  float textSize;
};

class Shaper {
 public:
  struct Result {
    Result(const FontFallback::Font& font,
           uint32_t glyph_id,
           uint32_t cluster_idx_,
           float x_advance,
           float y_advance,
           float x_offset,
           float y_offset,
           float x_bearing,
           float y_bearing,
           float width,
           float height)
        : font_(font),
          glyph_id_(glyph_id),
          cluster_idx_(cluster_idx_),
          x_advance_(x_advance),
          y_advance_(y_advance),
          x_offset_(x_offset),
          y_offset_(y_offset),
          x_bearing_(x_bearing),
          y_bearing_(y_bearing),
          width_(width),
          height_(height) {}

    const FontFallback::Font& font_;
    uint32_t glyph_id_;
    uint32_t cluster_idx_;
    float x_advance_;
    float y_advance_;
    float x_offset_;
    float y_offset_;
    float x_bearing_;
    float y_bearing_;
    float width_;
    float height_;

    std::string toString() const;
  };

  Shaper() : fallback_(FontFallback::createFallback("sans-serif")) {}
  ~Shaper() {}

  void init(float text_size);

  float textSize() const { return text_size_; }

  std::pair<std::vector<Result>, bool> shape(const std::vector<uint32_t>& text,
                                             Range range,
                                             const Rasterizer& rasterizer,
                                             bool isRtl) const;
  const FontFallback::Font& defaultFont() const {
    return fallback_.defaultFont();
  }

  float text_size_;
  FontFallback fallback_;
};
