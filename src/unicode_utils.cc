#include "unicode_utils.h"

#include <unicode/utf8.h>

std::vector<uint32_t> utf8ToCodePoints(const std::string& text) {
  std::vector<uint32_t> code_points;
  int32_t len = text.size();
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(text.data());
  for (int32_t i = 0; i < len;) {
    uint32_t c = 0;
    U8_NEXT(ptr, i, len, c);
    code_points.push_back(c);
  }
  return code_points;
}

std::vector<uint16_t> codePointsToUtf16(
    const std::vector<uint32_t>& code_points) {
  std::vector<uint16_t> utf16;
  for (uint32_t c : code_points) {
    if (U16_IS_SURROGATE(c)) {
      utf16.push_back(U16_LEAD(c));
      utf16.push_back(U16_TRAIL(c));
    } else {
      utf16.push_back(static_cast<uint16_t>(c));
    }
  }
  return utf16;
}
