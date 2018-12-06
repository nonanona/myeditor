#pragma once

#include <cstdint>
#include <vector>

namespace text_util {

std::vector<std::vector<uint32_t>> split(std::vector<uint32_t> str,
                                         uint32_t delimiter);

}  // namespace text_util
