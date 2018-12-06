#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "hb_util.h"
#include "mapped_buffer.h"
#include "range.h"

class FontFallback {
 public:
  struct Font {
    struct Metrics {
      float ascender;
      float descender;
      float line_gap;
    };

    Font(const char* path) : Font(path, 0) {}
    Font(const char* path, uint32_t index);

    uint32_t id;
    const char* path;
    uint32_t index;
    MappedBuffer buffer;
    HbFontUniquePtr font;

    Metrics metrics(float text_size) const;

   private:
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
    Font(Font&&) = delete;
    Font& operator=(Font&&) = delete;
  };

  static FontFallback createFallback(const std::string& name);

  struct Run {
    const Font& font;
    uint32_t length;
  };

  std::vector<Run> itemize(const std::vector<uint32_t>& input,
                           Range range) const;

  const Font& defaultFont() const { return fonts_[0]; }

 private:
  FontFallback(const Font* fonts, uint32_t length)
      : fonts_(fonts), fallback_length_(length) {}

  const Font* fonts_;  // Not taking ownership
  uint32_t fallback_length_;
};
