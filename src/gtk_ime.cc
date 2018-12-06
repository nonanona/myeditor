#include <glib-object.h>
#include <glib.h>
#include <gobject/gvaluecollector.h>

#include "gtk_ime.h"

GtkInputMethod::GtkInputMethod(IMEListener* listener)
    : ctx_(gtk_im_multicontext_new()), listener_(listener) {
  g_signal_connect(ctx_, "commit",
                   G_CALLBACK(+[](GtkIMContext* ctx, gchar* str, void* ptr) {
                     reinterpret_cast<GtkInputMethod*>(ptr)->onCommitText(str);
                   }),
                   this);
  g_signal_connect(
      ctx_, "delete-surrounding",
      G_CALLBACK(+[](GtkIMContext* ctx, gint start, gint n_chars, void* ptr) {
        reinterpret_cast<GtkInputMethod*>(ptr)->onDeleteSurroundingText(
            Range(start, start + n_chars));
      }),
      this);
  g_signal_connect(
      ctx_, "preedit-changed", G_CALLBACK(+[](GtkIMContext* ctx, void* ptr) {
        gchar* g_str = nullptr;
        PangoAttrList* attrs = nullptr;
        gint cursor_pos = 0;
        gtk_im_context_get_preedit_string(ctx, &g_str, &attrs, &cursor_pos);
        std::string str(g_str);
        // TODO: use atttrs and cursor_pos
        g_free(reinterpret_cast<gpointer>(g_str));
        pango_attr_list_unref(attrs);
        reinterpret_cast<GtkInputMethod*>(ptr)->onCompositionUpdate(str);
      }),
      this);
  g_signal_connect(ctx_, "preedit-end",
                   G_CALLBACK(+[](GtkIMContext* ctx, void* ptr) {
                     reinterpret_cast<GtkInputMethod*>(ptr)->onCompositionEnd();
                   }),
                   this);
  g_signal_connect(
      ctx_, "preedit-start", G_CALLBACK(+[](GtkIMContext* ctx, void* ptr) {
        reinterpret_cast<GtkInputMethod*>(ptr)->onCompositionStart();
      }),
      this);
}

void GtkInputMethod::focus() {
  gtk_im_context_focus_in(ctx_);
}

void GtkInputMethod::blur() {
  gtk_im_context_focus_out(ctx_);
}

void GtkInputMethod::reset() {
  gtk_im_context_reset(ctx_);
}

bool GtkInputMethod::sendKeyEvent(const KeyEvent& key) {
  return gtk_im_context_filter_keypress(ctx_, key.key_event);
}

void GtkInputMethod::onCommitText(const std::string& text) {
  listener_->onCommitText(text);
}

void GtkInputMethod::onCompositionStart() {
  listener_->onCompositionStart();
}

void GtkInputMethod::onCompositionUpdate(const std::string& text) {
  listener_->onCompositionUpdate(text);
}

void GtkInputMethod::onCompositionEnd() {
  listener_->onCompositionEnd();
}

void GtkInputMethod::onDeleteSurroundingText(Range range) {
  listener_->onDeleteSurroundingText(range);
}
