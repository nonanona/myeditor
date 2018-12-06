#include "shaper.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>

#include <hb-ot.h>
#include <hb.h>

#include "hb_util.h"
#include "log.h"
#include "text_itemizer.h"

namespace {
std::vector<Range> split(const std::vector<uint32_t>& code_points,
                         Range range,
                         uint32_t delim) {
  if (range.empty()) {
    return std::vector<Range>();
  }
  std::vector<uint32_t> offsets;
  offsets.push_back(range.start());
  for (uint32_t i = range.start(); i < range.end(); ++i) {
    if (code_points[i] == delim) {
      if (offsets.back() != i) {
        offsets.push_back(i);
      }
      offsets.push_back(i + 1);
    }
  }
  if (offsets.back() != range.end()) {
    offsets.push_back(range.end());
  }
  std::vector<Range> result;
  for (uint32_t i = 0; i < offsets.size() - 1; ++i) {
    result.push_back(Range(offsets[i], offsets[i + 1]));
  }

  return result;
};

static float HBFixedToFloat(hb_position_t v) {
  return scalbnf(v, -8);
}

static hb_position_t HBFloatToFixed(float v) {
  return scalbnf(v, +8);
}

}  // namespace
void Shaper::init(float text_size) {
  text_size_ = text_size;
}

std::pair<std::vector<Shaper::Result>, bool> Shaper::shape(
    const std::vector<uint32_t>& code_points,
    Range range,
    const Rasterizer& rasterizer,
    bool isRtl) const {
  // Set script/language
  std::vector<Result> results;

  BidiResult res = resolveBidi(code_points, range, isRtl);
  if (res.paraIsRtl) {
    std::reverse(res.runs.begin(), res.runs.end());
  }
  for (auto [bidiRunRange, isRtl] : res.runs) {
    std::vector<Range> words = split(code_points, bidiRunRange, ' ');
    if (isRtl) {
      std::reverse(words.begin(), words.end());
    }
    for (Range range : words) {
      std::vector<FontFallback::Run> runs =
          fallback_.itemize(code_points, range);
      uint32_t start_offset = range.start();
      for (auto run : runs) {
        hb_font_set_ppem(run.font.font.get(), text_size_, text_size_);
        hb_font_set_scale(run.font.font.get(), HBFloatToFixed(text_size_),
                          HBFloatToFixed(text_size_));
        HbBufferUniquePtr buf(hb_buffer_create());
        hb_buffer_set_direction(buf.get(),
                                isRtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
        hb_buffer_add_codepoints(buf.get(), code_points.data(),
                                 code_points.size(), start_offset, run.length);
        hb_buffer_guess_segment_properties(buf.get());

        hb_shape(run.font.font.get(), buf.get(), nullptr /* feature */, 0);
        uint32_t num_glyphs = hb_buffer_get_length(buf.get());
        hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buf.get(), nullptr);
        hb_glyph_position_t* positions =
            hb_buffer_get_glyph_positions(buf.get(), nullptr);

        for (uint32_t i = 0; i < num_glyphs; ++i) {
          hb_glyph_extents_t ext = {};
          if (hb_font_get_glyph_extents(run.font.font.get(), infos[i].codepoint,
                                        &ext)) {
            results.emplace_back(run.font, infos[i].codepoint,
                                 roundf(HBFixedToFloat(positions[i].x_advance)),
                                 roundf(HBFixedToFloat(positions[i].y_advance)),
                                 roundf(HBFixedToFloat(positions[i].x_offset)),
                                 roundf(HBFixedToFloat(positions[i].y_offset)),
                                 roundf(HBFixedToFloat(ext.x_bearing)),
                                 roundf(HBFixedToFloat(ext.y_bearing)),
                                 roundf(HBFixedToFloat(ext.width)),
                                 roundf(HBFixedToFloat(ext.height)));
          } else {
            IRect rect =
                rasterizer.bbox(run.font, infos[i].codepoint, text_size_);
            results.emplace_back(run.font, infos[i].codepoint,
                                 roundf(HBFixedToFloat(positions[i].x_advance)),
                                 roundf(HBFixedToFloat(positions[i].y_advance)),
                                 roundf(HBFixedToFloat(positions[i].x_offset)),
                                 roundf(HBFixedToFloat(positions[i].y_offset)),
                                 rect.l(), rect.t(), rect.width(),
                                 rect.height() * -1.0f);
          }
        }
        start_offset += run.length;
      }
    }
  }
  return std::make_pair(results, res.paraIsRtl);
  ;
}

std::string Shaper::Result::toString() const {
  char buf[64] = {};
  std::string out = "Shaper::Result{";
  snprintf(buf, 64, "glyph ID: %d", glyph_id_);
  out += buf;
  out += ", ";
  snprintf(buf, 64, "advance: (%g, %g)", x_advance_, y_advance_);
  out += buf;
  out += ", ";
  snprintf(buf, 64, "offset: (%g, %g)", x_offset_, y_offset_);
  out += buf;
  out += ", ";
  snprintf(buf, 64, "bearing (%g, %g)", x_bearing_, y_bearing_);
  out += buf;
  out += ", ";
  snprintf(buf, 64, "size %gx%g", width_, height_);
  out += buf;
  out += "}";
  return out;
}
