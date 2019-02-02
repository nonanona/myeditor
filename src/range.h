#pragma once

#include <cstdint>

#define INVALID_OFFSET 0xFFFFFFFF

class Range {
 public:
  Range() : start_(INVALID_OFFSET), end_(INVALID_OFFSET) {}
  Range(uint32_t start, uint32_t end) : start_(start), end_(end) {}

  bool isValid() const {
    return start_ != INVALID_OFFSET && end_ != INVALID_OFFSET;
  }

  uint32_t start() const { return start_; }
  uint32_t end() const { return end_; }
  uint32_t length() const { return end_ - start_; }
  bool empty() const { return start_ == end_; }

  bool contains(uint32_t offset) const {
    return start() <= offset && offset < end();
  }

  bool contains(const Range& r) const {
    return start() <= r.start() && r.end() <= end();
  }

  bool intersect(const Range& r) const {
    return start() < r.end() && r.start() < end();
  }

  Range intersection(const Range& r) const {
    return Range(std::max(start(), r.start()), std::min(end(), r.end()));
  }

 private:
  uint32_t start_;
  uint32_t end_;
};
