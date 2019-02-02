#pragma once

#include <algorithm>

template<typename T>
T clamp(T val, T min_val, T max_val) {
  return std::min(max_val, std::max(min_val, val));
}
