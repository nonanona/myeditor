#pragma once

#include <hb.h>

#include <optional>
#include <unordered_map>
#include <vector>

#include "rasterizer.h"
#include "rect.h"
#include "shaper.h"

class SpliteGenerator {
 public:
  struct Key {
    Key(const FontFallback::Font& font, uint32_t glyph_id, float size)
        : font_id(font.id), glyph_id(glyph_id), size(size) {}

    uint32_t font_id;
    uint32_t glyph_id;
    float size;

    bool operator==(const Key& o) const {
      return font_id == o.font_id && glyph_id == o.glyph_id && size == o.size;
    }

    bool operator!=(const Key& o) const { return !(*this == o); }

    struct Hasher {
      std::size_t operator()(const SpliteGenerator::Key& k) const {
        return std::hash<uint32_t>()(k.font_id) ^
               (std::hash<uint32_t>()(k.glyph_id)) ^
               (std::hash<float>()(k.size));
      }
    };
  };

  struct Entry {
    Entry(IRect rect, FRect uv_rect, Shaper::Result glyph)
        : rect(rect), uv_rect(uv_rect), glyph(glyph) {}
    IRect rect;
    FRect uv_rect;  // rect in the cache in uv coordinate
    Shaper::Result glyph;

    std::string toString() const {
      return "rect: " + rect.toString() + ", glyph:" + glyph.toString();
    }
  };

  SpliteGenerator() : capacity_(ISize(1024, 1024)) {}

  ~SpliteGenerator() {}

  uint32_t flp2(uint32_t x) {
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return x - (x >> 1);
  }

  const ISize& capacity() const { return capacity_; }

  std::optional<Entry> lookup(const SpliteGenerator::Key& key) const {
    Key::Hasher hasher;
    auto r = map_.find(key);
    if (r != map_.end()) {
      return r->second;
    } else {
      return std::nullopt;
    }
  }

  const Entry& store(const SpliteGenerator::Key& key,
                     ISize size,
                     Shaper::Result);

 private:
  ISize capacity_;
  std::unordered_map<Key, Entry, Key::Hasher> map_;
  Rasterizer rasterizer_;
};
