#pragma once

#include "rect.h"

class Option {
 public:
  static const Option& get();
  static void init(int argc, char** argv);

  bool isVerboseMode() const { return verbose_; }
  bool isDebugMode() const { return debug_; }
  float getTextSizePt() const { return text_size_pt_; }
  const ISize& getWindowSize() const { return window_size_; }

 private:
  Option() {}

  static Option& getInstance();

  bool verbose_ = false;
  bool debug_ = false;
  float text_size_pt_ = 18.0f;
  ISize window_size_ = ISize(640, 480);
};
