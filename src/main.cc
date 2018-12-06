#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <string>
#include <unordered_map>

#include "editor.h"
#include "keyevent.h"
#include "log.h"
#include "option_parser.h"
#include "time_util.h"

#include "gtk_window.h"
#include "window.h"

class EditorCallback : public EditorWindow::Callback {
 public:
  virtual void onWindowCreated(ISize size,
                               EGLDisplay display,
                               EGLContext context,
                               EGLSurface surface,
                               FSize dpi) override {
    IRect rect(IPoint(0, 0), size);
    Border border = {10, 10, 10, 10};
    Padding padding = {0, 0, 0, 0};
    editor_ = std::make_unique<Editor>(display, context, surface, rect, border,
                                       padding, dpi);
    editor_->init(Option::get().getTextSizePt());
  }

  virtual void onDraw() override { editor_->onDraw(); }

  virtual void onCursorDraw() override { editor_->onCursorDraw(); }

  virtual void onKeyDown(const KeyEvent& key) override {
    editor_->onKeyEvent(key);
  }

  virtual void onKeyUp(const KeyEvent& key) override {
    //    editor_->onKeyUp(key);
  }

  virtual void onCommitText(const std::vector<uint32_t>& code_points) override {
    editor_->insertText(code_points);
  }

 private:
  std::unique_ptr<Editor> editor_;
};

int main(int argc, char** argv) {
  Option::init(argc, argv);
  gtk_init(&argc, &argv);

  ISize window_size = Option::get().getWindowSize();
  Border border = {10, 10, 10, 10};
  Padding padding = {0, 0, 0, 0};

  EditorWindow window(std::make_unique<GtkNativeWindow>(),
                      std::make_unique<EditorCallback>());
  window.init("Editor", window_size);
  window.show();

  gtk_main();
}
