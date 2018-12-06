#include "gtk_window.h"

#include <unicode/utf8.h>

#include "log.h"
#include "unicode_utils.h"

void GtkNativeWindow::init(EditorWindow::Callback* callback,
                           const std::string& title,
                           ISize size) {
  callback_ = callback;
  size_ = size;
  title_ = title;
  widget_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(widget_, size.width(), size.height());
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  gtk_widget_set_double_buffered(GTK_WIDGET(widget_), FALSE);
#pragma clang diagnostic pop
  g_signal_connect(G_OBJECT(widget_), "realize",
                   G_CALLBACK(+[](GtkWidget* widget, void* ptr) {
                     reinterpret_cast<GtkNativeWindow*>(ptr)->onRealize(widget);
                   }),
                   this);
  g_signal_connect(G_OBJECT(widget_), "draw",
                   G_CALLBACK(+[](GtkWidget* widget, void* ctx, void* ptr) {
                     reinterpret_cast<GtkNativeWindow*>(ptr)->onDraw(widget);
                     return TRUE;
                   }),
                   this);
  g_signal_connect(G_OBJECT(widget_), "key-press-event",
                   G_CALLBACK(+[](GtkWidget* widget, GdkEvent* e, void* ptr) {
                     reinterpret_cast<GtkNativeWindow*>(ptr)->onKeyDown(
                         widget, (GdkEventKey*)e);
                     return TRUE;
                   }),
                   this);
  g_signal_connect(G_OBJECT(widget_), "key-release-event",
                   G_CALLBACK(+[](GtkWidget* widget, GdkEvent* e, void* ptr) {
                     reinterpret_cast<GtkNativeWindow*>(ptr)->onKeyUp(
                         widget, (GdkEventKey*)e);
                     return TRUE;
                   }),
                   this);
  g_signal_connect(G_OBJECT(widget_), "focus-in-event",
                   G_CALLBACK(+[](GtkWidget* widget, GdkEvent* e, void* ptr) {
                     reinterpret_cast<GtkNativeWindow*>(ptr)->onFocusIn(
                         widget, (GdkEventFocus*)e);
                     return TRUE;
                   }),
                   this);
  g_signal_connect(G_OBJECT(widget_), "focus-out-event",
                   G_CALLBACK(+[](GtkWidget* widget, GdkEvent* e, void* ptr) {
                     reinterpret_cast<GtkNativeWindow*>(ptr)->onFocusOut(
                         widget, (GdkEventFocus*)e);
                     return TRUE;
                   }),
                   this);
  g_timeout_add(500 /* ms */, (GSourceFunc)(+[](void* ptr) {
                  reinterpret_cast<GtkNativeWindow*>(ptr)->onCursorDraw();
                  return TRUE;
                }),
                this);

  ime_ = std::make_unique<GtkInputMethod>(this);
}

void GtkNativeWindow::show() {
  gtk_widget_show(widget_);
}

void GtkNativeWindow::onRealize(GtkWidget* widget) {
  Display* dpy = gdk_x11_display_get_xdisplay(gtk_widget_get_display(widget));
  CHECK(dpy);
  Window win = gdk_x11_window_get_xid(gtk_widget_get_window(widget));

  egl_display_ = eglGetDisplay((EGLNativeDisplayType)dpy);
  CHECK(egl_display_ != EGL_NO_DISPLAY);
  CHECK(eglInitialize(egl_display_, nullptr, nullptr));

  EGLint attr[] = {EGL_BUFFER_SIZE, 16, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                   EGL_NONE};
  EGLConfig config = nullptr;
  EGLint numConfigs = 0;
  CHECK(eglChooseConfig(egl_display_, attr, &config, 1, &numConfigs));
  CHECK(numConfigs == 1);
  eglBindAPI(EGL_OPENGL_ES_API);
  egl_surface_ = eglCreateWindowSurface(egl_display_, config, win, nullptr);
  CHECK(egl_surface_ != EGL_NO_SURFACE);

  EGLint ctxattr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  egl_context_ =
      eglCreateContext(egl_display_, config, EGL_NO_CONTEXT, ctxattr);
  CHECK(egl_context_ != EGL_NO_CONTEXT);

  eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_);

  dpi_ = FSize(
      (((float)DisplayWidth(dpy, 0)) * 25.4f) / (float)DisplayWidthMM(dpy, 0),
      (((float)DisplayHeight(dpy, 0)) * 25.4f) / (float)DisplayWidthMM(dpy, 0));

  callback_->onWindowCreated(size_, egl_display_, egl_context_, egl_surface_,
                             dpi_);
}

void GtkNativeWindow::onDraw(GtkWidget* widget) {
  callback_->onDraw();
}

void GtkNativeWindow::onKeyDown(GtkWidget* widget, GdkEventKey* event) {
  KeyEvent e(event);
  if (!ime_->sendKeyEvent(e)) {
    callback_->onKeyDown(e);
  }
  gtk_widget_queue_draw(widget);
}

void GtkNativeWindow::onKeyUp(GtkWidget* widget, GdkEventKey* event) {
  callback_->onKeyUp(KeyEvent(event));
  gtk_widget_queue_draw(widget);
}

void GtkNativeWindow::onCursorDraw() {
  callback_->onCursorDraw();
}

void GtkNativeWindow::onFocusIn(GtkWidget* widget, GdkEventFocus* event) {
  ime_->focus();
}

void GtkNativeWindow::onFocusOut(GtkWidget* widget, GdkEventFocus* event) {
  ime_->blur();
}

IMEController* GtkNativeWindow::getImeController() {
  return ime_.get();
}

void GtkNativeWindow::onCommitText(const std::string& text) {
  callback_->onCommitText(utf8ToCodePoints(text));
}

void GtkNativeWindow::onCompositionStart() {
  printf("%s (%s:%d)\n", __func__, __FILE__, __LINE__);
}

void GtkNativeWindow::onCompositionUpdate(const std::string& text) {
  printf("%s (%s:%d) %s\n", __func__, __FILE__, __LINE__, text.c_str());
}

void GtkNativeWindow::onCompositionEnd() {
  printf("%s (%s:%d)\n", __func__, __FILE__, __LINE__);
}

void GtkNativeWindow::onDeleteSurroundingText(Range range) {
  printf("%s (%s:%d)\n", __func__, __FILE__, __LINE__);
}
