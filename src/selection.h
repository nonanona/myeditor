#pragma once

#include "log.h"
#include "range.h"
#include "math_util.h"
#include <stdio.h>

class Selection {
 public:
  Selection(): start_offset_(0), end_offset_(0) {}
  int32_t start() const { return start_offset_; }
  int32_t end() const { return end_offset_; }

  void collapse(int32_t offset) { start_offset_ = end_offset_ = offset; }
  void set(int32_t start_offset, int32_t end_offset) {
    start_offset_ = start_offset;
    end_offset_ = end_offset;
    fixReversedSelection();
  }

  bool isCollapsed() const { return start() == end(); }

  void move(int32_t amount, int32_t length) {
    CHECK(isCollapsed());
    int32_t new_val = clamp(start_offset_ + amount, 0, length);
    start_offset_ = end_offset_ = new_val;
  }

  void collapseToStart() {
    end_offset_ = start_offset_;
  }

  void collapseToEnd() {
    start_offset_ = end_offset_;
  }

  void extend(int32_t start_amount, int32_t end_amount, int32_t length) {
    start_offset_ = clamp(start_offset_ + start_amount, 0, length);
    end_offset_ = clamp(end_offset_ + end_amount, 0, length);
    fixReversedSelection();
  }

  Range toRange() const { return Range(start(), end()); }

  std::string toString() const {
    char buf[64] = {};
    if (isCollapsed()) {
      snprintf(buf, 64, "Collapsed: %d", start());
    } else {
      snprintf(buf, 64, "Selection: (%d, %d)", start(), end());
    }
    return buf;
  }

 private:
  int32_t start_offset_ = 0;
  int32_t end_offset_ = 0;

  void fixReversedSelection() {
    if (start_offset_ > end_offset_) {
      int32_t tmp = start_offset_;
      start_offset_ = end_offset_;
      end_offset_ = tmp;
    }
  }
};
