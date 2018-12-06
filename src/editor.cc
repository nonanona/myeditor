#include "editor.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <GLES2/gl2.h>

#include "clipboard.h"
#include "range.h"
#include "rasterizer.h"
#include "unicode_utils.h"

#define WIDTH 320
#define HEIGHT 240

Editor::Editor(EGLDisplay display,
               EGLContext context,
               EGLSurface surface,
               IRect rect,
               Border border,
               Padding padding,
               FSize dpi)
    : renderer_(display, context, surface, rect, border, padding), dpi_(dpi) {}

void Editor::init(float text_size_pt) {
  renderer_.init(text_size_pt * dpi_.width() / 72.0);
}

bool Editor::onKeyEvent(const KeyEvent& key) {
  if (key.hasModifier()) {
    return performEditorCommands(key);
  }

  if (isascii((int)key.keysym)) {
    uint32_t tail = text_.lengthInCodePoints();
    text_.replace(Range(cursor_offset_, cursor_offset_),
                  std::vector<uint32_t>({static_cast<uint32_t>(key.keysym)}));
    cursor_offset_++;
  } else if (key.keysym == KeySymbol::KEY_BACKSPACE) {
    if (cursor_offset_ != 0) {
      text_.replace(Range(cursor_offset_ - 1, cursor_offset_),
                    std::vector<uint32_t>());
      cursor_offset_ = cursor_offset_ == 0 ? 0 : cursor_offset_ - 1;
    }
  } else if (key.keysym == KeySymbol::KEY_DELETE) {
    if (cursor_offset_ != text_.code_points().size()) {
      text_.replace(Range(cursor_offset_, cursor_offset_ + 1),
                    std::vector<uint32_t>());
    }
  } else if (isArrowKey(key.keysym)) {
    // Handle grapheme clusters.
    switch (key.keysym) {
      case KeySymbol::KEY_LEFT:
        if (cursor_offset_ != 0) {
          cursor_offset_--;
        }
        break;
      case KeySymbol::KEY_RIGHT:
        if (cursor_offset_ != text_.code_points().size()) {
          cursor_offset_++;
        }
        break;
      case KeySymbol::KEY_UP:
      case KeySymbol::KEY_DOWN:
      default:
        printf("Unsupported keys\n");
    }
  } else if (key.keysym == KeySymbol::KEY_ENTER) {
    insertText(std::vector<uint32_t>({'\n'}));
  } else {
    printf("Unhandled Key: 0x%x\n", key.keysym);
  }
  return true;
}

void Editor::onDraw() {
  renderer_.renderText(text_, cursor_offset_);
}

void Editor::onCursorDraw() {
  renderer_.onCursorDraw();
}

void Editor::insertText(const std::vector<uint32_t>& code_points) {
  text_.replace(Range(cursor_offset_, cursor_offset_), code_points);
  cursor_offset_ += code_points.size();
  text_.dump();
}

bool Editor::performEditorCommands(const KeyEvent& key) {
  if (key.isModifier(Modifier::CTRL) &&
      (key.keysym == KeySymbol::KEY_V || key.keysym == KeySymbol::KEY_v)) {
    requestClipboardText(
        [=](const std::string& text) { insertText(utf8ToCodePoints(text)); });
  }
  return true;
}
