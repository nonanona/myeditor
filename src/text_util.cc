#include "text_util.h"

namespace text_util {

std::vector<std::vector<uint32_t>> split(std::vector<uint32_t> str,
                                         uint32_t delimiter) {
  std::vector<std::vector<uint32_t>> result;
  std::vector<uint32_t> element;
  for (uint32_t i = 0; i < str.size(); ++i) {
    element.push_back(str[i]);
    if (str[i] == delimiter) {
      result.push_back(element);
      element.clear();
    }
  }
  return result;
}

}  // namespace text_util
