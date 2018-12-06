#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include "gtk_ime.h"
#include "ime_listener.h"
#include "window.h"

class GtkNativeWindow : public EditorWindow::Delegate, public IMEListener {
 public:
  GtkNativeWindow() {}
  virtual ~GtkNativeWindow() {}

  virtual void init(EditorWindow::Callback* callback,
                    const std::string& title,
                    ISize size) override;
  virtual void show() override;

  virtual IMEController* getImeController() override;

  virtual void onCommitText(const std::string& text) override;

  virtual void onCompositionStart() override;
  virtual void onCompositionUpdate(const std::string& text) override;
  virtual void onCompositionEnd() override;

  virtual void onDeleteSurroundingText(Range range) override;

 private:
  // Signal callbacks
  void onDraw(GtkWidget* widget);
  void onCursorDraw();
  void onRealize(GtkWidget* widget);
  void onKeyDown(GtkWidget* widget, GdkEventKey* event);
  void onKeyUp(GtkWidget* widget, GdkEventKey* event);
  void onFocusIn(GtkWidget* widget, GdkEventFocus* event);
  void onFocusOut(GtkWidget* widget, GdkEventFocus* event);
  void onClipboardTextArrivied(const std::string& text);

  EditorWindow::Callback* callback_;

  std::string title_;
  ISize size_;

  GtkWidget* widget_;

  EGLDisplay egl_display_;
  EGLSurface egl_surface_;
  EGLContext egl_context_;

  FSize dpi_;

  std::unique_ptr<GtkInputMethod> ime_;
};
