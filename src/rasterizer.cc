#include "rasterizer.h"

#include <cmath>
#include <vector>

#include "hb_util.h"
#include "log.h"
#include "option_parser.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace {

const FT_Int32 kLoadFlags = FT_LOAD_DEFAULT | FT_LOAD_TARGET_NORMAL |
                            FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

constexpr float FTPosToFloat(FT_Pos x) {
  return x / 64.0f;
}

constexpr FT_F26Dot6 FTFloatToF26Dot6(float x) {
  return static_cast<FT_F26Dot6>(x * 64);
}

}  // namespace

void A8Bitmap::saveToFile(const std::string& file) const {
  int fd = open(file.c_str(), O_CREAT | O_RDWR, 0644);
  CHECK(fd != -1);
  dprintf(fd, "P2\n");
  dprintf(fd, "%d %d\n", w_, h_);
  dprintf(fd, "255\n");
  for (int y = 0; y < h_; ++y) {
    for (int x = 0; x < w_; ++x) {
      dprintf(fd, "%d ", 255 - buffer_[y * w_ + x]);
    }
    dprintf(fd, "\n");
  }
  dprintf(fd, "\n");
  close(fd);
}

Rasterizer::Rasterizer() {
  if (FT_Init_FreeType(&library_) != 0) {
    printf("Failed to init library.\n");
    exit(1);
  }
}

A8Bitmap Rasterizer::rasterize(const FontFallback::Font& font,
                               uint32_t glyph_id,
                               float size) const {
  FT_Open_Args args = {};
  args.flags = FT_OPEN_MEMORY;

  args.memory_base = font.buffer.data();
  args.memory_size = font.buffer.size();

  FT_Face raw_face;
  if (FT_Open_Face(library_, &args, font.index, &raw_face) != 0) {
    printf("Failed to open face\n");
    exit(1);
  }
  FTFaceUniquePtr face(raw_face);

  const FT_F26Dot6 scale = FTFloatToF26Dot6(size);
  FT_Set_Pixel_Sizes(face.get(), (int)(size), (int)(size));
  FT_Load_Glyph(face.get(), glyph_id, kLoadFlags);

  FT_Outline* outline = &face.get()->glyph->outline;

  FT_BBox bbox = {};
  FT_Outline_Get_CBox(outline, &bbox);

  FT_Outline_Translate(outline, -(bbox.xMin & ~63), -(bbox.yMin & ~63));

  int left = FTPosToFloat(bbox.xMin);
  int right = FTPosToFloat(bbox.xMax);
  int top = FTPosToFloat(bbox.yMin);
  int bottom = FTPosToFloat(bbox.yMax);

  int width = ceil(FTPosToFloat(bbox.xMax - bbox.xMin));
  int height = ceil(FTPosToFloat(bbox.yMax - bbox.yMin));

  if (Option::get().isVerboseMode()) {
    printf("FT BBox : %g(%d),%g(%d) - %g(%d),%g(%d), %g(%d)x%g(%d)\n",
           FTPosToFloat(bbox.xMin), left, FTPosToFloat(bbox.yMin), top,
           FTPosToFloat(bbox.xMax), right, FTPosToFloat(bbox.yMax), bottom,
           FTPosToFloat(bbox.xMax - bbox.xMin), width,
           FTPosToFloat(bbox.yMax - bbox.yMin), height);
  }

  A8Bitmap res(width, height);

  FT_Bitmap target = {};
  target.width = width;
  target.rows = height;
  target.pitch = res.rowBytes();
  target.buffer = res.buffer();
  target.pixel_mode = FT_PIXEL_MODE_GRAY;
  target.num_grays = 256;

  FT_Outline_Get_Bitmap(face.get()->glyph->library, outline, &target);

  return res;
}

IRect Rasterizer::bbox(const FontFallback::Font& font,
                       uint32_t glyph_id,
                       float size) const {
  FT_Open_Args args = {};
  args.flags = FT_OPEN_MEMORY;

  args.memory_base = font.buffer.data();
  args.memory_size = font.buffer.size();

  FT_Face raw_face;
  if (FT_Open_Face(library_, &args, font.index, &raw_face) != 0) {
    printf("Failed to open face\n");
    exit(1);
  }
  FTFaceUniquePtr face(raw_face);

  const FT_F26Dot6 scale = FTFloatToF26Dot6(size);
  FT_Set_Pixel_Sizes(face.get(), (int)(size), (int)(size));
  FT_Load_Glyph(face.get(), glyph_id, kLoadFlags);

  FT_Outline* outline = &face.get()->glyph->outline;

  FT_BBox bbox = {};
  FT_Outline_Get_CBox(outline, &bbox);

  int left = FTPosToFloat(bbox.xMin);
  int right = FTPosToFloat(bbox.xMax);
  int top = FTPosToFloat(bbox.yMin);
  int bottom = FTPosToFloat(bbox.yMax);

  int width = ceil(FTPosToFloat(bbox.xMax - bbox.xMin));
  int height = ceil(FTPosToFloat(bbox.yMax - bbox.yMin));

  return IRect(IPoint(left, top + height), ISize(width, height));
}
