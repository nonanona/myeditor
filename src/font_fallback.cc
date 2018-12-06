#include "font_fallback.h"

#include <atomic>
#include <cmath>

#include <hb-ot.h>

#define NOTO_FONT_DIR "./src/third_party/noto-fonts/"
#define ROBOTO_FONT_DIR "./src/third_party/roboto-fonts/"

namespace {
std::atomic<uint32_t> gNextFontId = {0};

static float HBFixedToFloat(hb_position_t v) {
  return scalbnf(v, -8);
}

static hb_position_t HBFloatToFixed(float v) {
  return scalbnf(v, +8);
}

};  // namespace

FontFallback::Font SANS_SERIF[] = {
    FontFallback::Font(ROBOTO_FONT_DIR "Roboto-Regular.ttf"),
    FontFallback::Font(NOTO_FONT_DIR "NotoSansCJK-Regular.ttc", 1),
    FontFallback::Font(NOTO_FONT_DIR "NotoNaskhArabic-Regular.ttf"),
};

template <typename T, int size>
uint32_t arraysize(T (&)[size]) {
  return size;
}

FontFallback::Font::Font(const char* path, uint32_t index)
    : id(gNextFontId++), path(path), index(index) {
  buffer = MappedBuffer::create(path);
  HbBlobUniquePtr blob(
      hb_blob_create(reinterpret_cast<const char*>(buffer.data()),
                     buffer.size(), HB_MEMORY_MODE_READONLY, nullptr, nullptr));
  HbFaceUniquePtr face(hb_face_create(blob.get(), index));

  font = HbFontUniquePtr(hb_font_create(face.get()));
  unsigned int upem = hb_face_get_upem(face.get());
  hb_font_set_scale(font.get(), upem, upem);

  hb_ot_font_set_funcs(font.get());
}

FontFallback::Font::Metrics FontFallback::Font::metrics(float text_size) const {
  hb_font_set_ppem(font.get(), text_size, text_size);
  hb_font_set_scale(font.get(), HBFloatToFixed(text_size),
                    HBFloatToFixed(text_size));
  hb_font_extents_t ext = {};
  hb_font_get_h_extents(font.get(), &ext);
  return Metrics({HBFixedToFloat(ext.ascender), HBFixedToFloat(ext.descender),
                  HBFixedToFloat(ext.line_gap)});
}

// static
FontFallback FontFallback::createFallback(const std::string& name) {
  return FontFallback(SANS_SERIF, arraysize(SANS_SERIF));
}

std::vector<FontFallback::Run> FontFallback::itemize(
    const std::vector<uint32_t>& input,
    Range range) const {
  std::vector<uint32_t> font_indices;
  std::vector<uint32_t> lengths;

  for (uint32_t i = range.start(); i < range.end(); ++i) {
    uint32_t cp = input[i];
    bool found = false;
    for (uint32_t j = 0; j < fallback_length_; ++j) {
      hb_codepoint_t glyph = 0;
      // TODO: support variation selector
      if (hb_font_get_glyph(fonts_[j].font.get(), cp, 0, &glyph)) {
        if (!font_indices.empty() && font_indices.back() == j) {
          lengths.back()++;
        } else {
          font_indices.push_back(j);
          lengths.push_back(1);
        }
        found = true;
        break;
      }
    }
    if (!found) {
      if (!font_indices.empty() && font_indices.back() == 0) {
        lengths.back()++;
      } else {
        font_indices.push_back(0);
        lengths.push_back(1);
      }
    }
  }
  std::vector<Run> result;
  for (int i = 0; i < font_indices.size(); ++i) {
    result.push_back(Run({fonts_[font_indices[i]], lengths[i]}));
  }
  return result;
}
