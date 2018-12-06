#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>

template <typename T, FT_Error (*F)(T*)>
struct FTDeleter {
  void operator()(T* v) { F(v); }
};

using FTFaceUniquePtr =
    std::unique_ptr<FT_FaceRec, FTDeleter<FT_FaceRec, FT_Done_Face>>;
