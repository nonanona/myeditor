#pragma once

#include <string>
#include "range.h"

class IMEListener {
 public:
  IMEListener() {}
  virtual ~IMEListener() {}

  virtual void onCommitText(const std::string& text) = 0;

  virtual void onCompositionStart() = 0;
  virtual void onCompositionUpdate(const std::string& text) = 0;
  virtual void onCompositionEnd() = 0;

  virtual void onDeleteSurroundingText(Range range) = 0;
};
