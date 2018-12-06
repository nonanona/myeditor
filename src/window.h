#pragma once

#include <memory>
#include <vector>
#include "ime_controller.h"
#include "keyevent.h"
#include "rect.h"

class EditorWindow {
 public:
  class Callback;
  class Delegate {
   public:
    Delegate() {}
    virtual ~Delegate() {}

    // not passing ownership
    virtual void init(Callback*, const std::string& title, ISize size) = 0;
    virtual void show() = 0;

    virtual IMEController* getImeController() = 0;
  };

  class Callback {
   public:
    Callback() {}
    virtual ~Callback() {}

    virtual void onWindowCreated(ISize window_size,
                                 EGLDisplay display,
                                 EGLContext context,
                                 EGLSurface surface,
                                 FSize dpi) = 0;
    virtual void onKeyDown(const KeyEvent& key) = 0;
    virtual void onKeyUp(const KeyEvent& key) = 0;
    virtual void onDraw() = 0;
    virtual void onCursorDraw() = 0;

    virtual void onCommitText(const std::vector<uint32_t>& code_points) = 0;
  };

  EditorWindow(std::unique_ptr<Delegate>&& delegate,
               std::unique_ptr<Callback>&& callback)
      : delegate_(std::move(delegate)), callback_(std::move(callback)) {}

  void init(const std::string& title, ISize size) {
    delegate_->init(callback_.get(), title, size);
  }
  void show() { delegate_->show(); }

 private:
  std::string title_;
  ISize size_;
  std::unique_ptr<Delegate> delegate_;
  std::unique_ptr<Callback> callback_;
  IMEController* ime_controller_;
};
