#include "clipboard.h"

#include <gtk/gtk.h>

struct CallbackObj {
  std::function<void(const std::string&)> callback;
};

void requestClipboardText(std::function<void(const std::string&)> callback) {
  GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  std::unique_ptr<CallbackObj> obj = std::make_unique<CallbackObj>();
  obj->callback = callback;

  gtk_clipboard_request_text(
      clipboard,
      (GtkClipboardTextReceivedFunc)(+[](GtkClipboard*, const gchar* text,
                                         void* ptr) {
        std::unique_ptr<CallbackObj> obj(reinterpret_cast<CallbackObj*>(ptr));
        obj->callback(text);
      }),
      obj.release());
}
