#pragma once

#include <gtk/gtk.h>
#include <gtk/gtkimmodule.h>

#include "ime_controller.h"
#include "ime_listener.h"

class GtkInputMethod : public IMEController {
 public:
  GtkInputMethod(IMEListener* listener);
  virtual ~GtkInputMethod() {}

  virtual void focus() override;
  virtual void blur() override;
  virtual void reset() override;
  virtual bool sendKeyEvent(const KeyEvent& key) override;

  void onCommitText(const std::string& text);
  void onCompositionStart();
  void onCompositionUpdate(const std::string& text);
  void onCompositionEnd();
  void onDeleteSurroundingText(Range range);

 private:
  GtkIMContext* ctx_;
  IMEListener* listener_;
};
