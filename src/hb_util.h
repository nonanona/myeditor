#pragma once

#include <hb.h>
#include <memory>

template <typename T, void (*F)(T*)>
struct HbDeleter {
  void operator()(T* v) { F(v); }
};

using HbBlobUniquePtr =
    std::unique_ptr<hb_blob_t, HbDeleter<hb_blob_t, hb_blob_destroy>>;
using HbFaceUniquePtr =
    std::unique_ptr<hb_face_t, HbDeleter<hb_face_t, hb_face_destroy>>;
using HbFontUniquePtr =
    std::unique_ptr<hb_font_t, HbDeleter<hb_font_t, hb_font_destroy>>;
using HbBufferUniquePtr =
    std::unique_ptr<hb_buffer_t, HbDeleter<hb_buffer_t, hb_buffer_destroy>>;

class HbBlob {
 public:
  HbBlob(const HbFaceUniquePtr& face, uint32_t tag)
      : mBlob(hb_face_reference_table(face.get(), tag)) {}
  HbBlob(const HbFaceUniquePtr& face)
      : mBlob(hb_face_reference_blob(face.get())) {}
  HbBlob(hb_face_t* face, uint32_t tag)
      : mBlob(hb_face_reference_table(face, tag)) {}
  HbBlob(hb_face_t* face) : mBlob(hb_face_reference_blob(face)) {}

  HbBlob(const HbFontUniquePtr& font, uint32_t tag)
      : mBlob(hb_face_reference_table(hb_font_get_face(font.get()), tag)) {}
  HbBlob(const HbFontUniquePtr& font)
      : mBlob(hb_face_reference_blob(hb_font_get_face(font.get()))) {}
  HbBlob(hb_font_t* font, uint32_t tag)
      : mBlob(hb_face_reference_table(hb_font_get_face(font), tag)) {}
  HbBlob(hb_font_t* font)
      : mBlob(hb_face_reference_blob(hb_font_get_face(font))) {}

  inline const uint8_t* get() const {
    return reinterpret_cast<const uint8_t*>(
        hb_blob_get_data(mBlob.get(), nullptr));
  }

  inline uint32_t size() const {
    return (uint32_t)(hb_blob_get_length(mBlob.get()));
  }

  inline operator bool() const { return size() > 0; }

 private:
  HbBlobUniquePtr mBlob;
};
