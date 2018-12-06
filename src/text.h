#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "range.h"

class Text {
 public:
  Text() {}
  Text(const std::string& utf8) : code_points_(utf8ToCodePoints(utf8)) {}
  Text(const std::vector<uint16_t> utf16)
      : code_points_(utf16ToCodePoints(utf16)) {}
  Text(const std::vector<uint32_t> codepoints) : code_points_(codepoints) {}

  void replace(const Range& range, const std::vector<uint32_t>& code_points);

  // Accessors
  const std::string& utf8() const;
  const std::vector<uint16_t> utf16() const;
  const std::vector<uint32_t> code_points() const { return code_points_; }

  uint32_t lengthInCodePoints() const { return code_points_.size(); }

  // Utilities
  static std::string codePointsToUtf8(const std::vector<uint32_t>& code_points);
  static std::string utf16ToUtf8(const std::vector<uint16_t>& code_points);
  static std::vector<uint16_t> codePointsToUtf16(
      const std::vector<uint32_t>& code_points);
  static std::vector<uint16_t> utf8ToUtf16(const std::string& code_points);
  static std::vector<uint32_t> utf8ToCodePoints(const std::string& code_points);
  static std::vector<uint32_t> utf16ToCodePoints(
      const std::vector<uint16_t>& code_points);

  std::string dump();

 private:
  std::vector<uint32_t> code_points_;  // source of truth of text

  // Following two representations are generated when necessary.
  // TODO: introduce mutex to guard these two for making thread-safe.
  std::optional<std::vector<uint16_t>> utf16_;
  std::optional<std::string> utf8_t;
};
