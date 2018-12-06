#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <unicode/ubidi.h>

template <typename T, void (*F)(T*)>
struct ICUDeleter {
  void operator()(T* v) { F(v); }
};

using UBiDiUniquePtr = std::unique_ptr<UBiDi, ICUDeleter<UBiDi, ubidi_close>>;

std::vector<uint32_t> utf8ToCodePoints(const std::string& text);
std::vector<uint16_t> codePointsToUtf16(
    const std::vector<uint32_t>& code_points);
