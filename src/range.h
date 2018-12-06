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

 private:
  uint32_t start_;
  uint32_t end_;
};
