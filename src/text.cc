#include "text.h"

/*
// static
std::string codePointsToUtf8(const std::vector<uint32_t>& code_points) {
}

// static
std::string utf16ToUtf8(const std::vector<uint16_t>& code_points) {
}

// static
std::vector<uint16_t> codePointsToUtf16(const std::vector<uint32_t>&
code_points) {
}

// static
std::vector<uint16_t> utf8ToUtf16(const std::string& code_points) {
}

// static
std::vector<uint32_t> utf8ToCodePoints(const std::string& code_points) {
}

// static
std::vector<uint32_t> utf16ToCodePoints(const std::vector<uint16_t>&
code_points) {
}
*/

void Text::replace(const Range& range,
                   const std::vector<uint32_t>& code_points) {
  std::vector<uint32_t> new_cp;
  // TODO: Stop doing stupid things
  for (uint32_t i = 0; i < range.start() && i < code_points_.size(); ++i) {
    new_cp.push_back(code_points_[i]);
  }

  for (uint32_t i = 0; i < code_points.size(); ++i) {
    new_cp.push_back(code_points[i]);
  }

  for (uint32_t i = range.end(); i < code_points_.size(); ++i) {
    new_cp.push_back(code_points_[i]);
  }

  code_points_ = new_cp;
}

std::string Text::dump() {
  std::string out = "Text: ";
  for (uint32_t c : code_points_) {
    char buf[32] = {};
    snprintf(buf, 32, "U+%04X, ", c);
    out += buf;
  }
  return out;
}
