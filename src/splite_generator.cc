#include "splite_generator.h"

#include <algorithm>

const SpliteGenerator::Entry& SpliteGenerator::store(
    const SpliteGenerator::Key& key,
    ISize size,
    Shaper::Result result) {
  uint32_t max_x = 0;
  for (auto [key, entry] : map_) {
    max_x = std::max(max_x, entry.rect.r());
  }
  if (capacity_.width() - max_x < size.width()) {
    printf("No space left in cache texture.\n");
    map_.clear();
    max_x = 0;
  }

  IRect rect(max_x, 0, size.width(), size.height());
  FRect uv_rect((float)max_x / (float)capacity_.width(),
                (float)0 / (float)capacity_.height(),
                (float)size.width() / (float)capacity_.width(),
                (float)size.height() / (float)capacity_.height());

  Entry entry(rect, uv_rect, result);
  auto [it, _] = map_.insert(std::make_pair(key, entry));
  return it->second;
}
