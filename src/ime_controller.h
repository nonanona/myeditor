#pragma once

#include "keyevent.h"

class IMEController {
 public:
  IMEController() {}
  virtual ~IMEController() {}

  virtual void focus() = 0;
  virtual void blur() = 0;
  virtual void reset() = 0;
  virtual bool sendKeyEvent(const KeyEvent& key) = 0;
};
