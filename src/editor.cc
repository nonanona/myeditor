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
  if (key.hasModifier(Modifier::CTRL)) {
    return performEditorCommands(key);
  }

  if (isascii((int)key.keysym)) {
    uint32_t tail = text_.lengthInCodePoints();
    text_.replace(selection_.toRange(),
                  std::vector<uint32_t>({static_cast<uint32_t>(key.keysym)}));
    selection_.move(1, text_.code_points().size());
  } else if (key.keysym == KeySymbol::KEY_BACKSPACE) {
    if (selection_.isCollapsed()) {
      if (selection_.start() != 0) {
        text_.replace(Range(selection_.start() - 1, selection_.start()),
                      std::vector<uint32_t>());
        selection_.move(-1, text_.code_points().size());
      }
    } else {
      text_.replace(selection_.toRange(), std::vector<uint32_t>());
    }
  } else if (key.keysym == KeySymbol::KEY_DELETE) {
    if (selection_.isCollapsed()) {
      if (selection_.start() != text_.code_points().size()) {
        text_.replace(Range(selection_.start(), selection_.end() + 1),
                      std::vector<uint32_t>());
      }
    } else {
      text_.replace(selection_.toRange(), std::vector<uint32_t>());
    }
  } else if (isArrowKey(key.keysym)) {
    // Handle grapheme clusters.
    const bool hasShift = key.hasModifier(Modifier::SHIFT);
    switch (key.keysym) {
      case KeySymbol::KEY_LEFT:
        if (hasShift) {
          selection_.extend(-1, 0, text_.code_points().size());
        } else {
          if (!selection_.isCollapsed()) {
            selection_.collapseToStart();  // opposit in RTL?
          }
          selection_.move(-1, text_.code_points().size());
        }
        break;
      case KeySymbol::KEY_RIGHT:
        if (hasShift) {
          selection_.extend(0, 1, text_.code_points().size());
        } else {
          if (!selection_.isCollapsed()) {
            selection_.collapseToEnd();
          }
          selection_.move(1, text_.code_points().size());
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
  renderer_.renderText(text_, selection_);
}

void Editor::onCursorDraw() {
  renderer_.onCursorDraw();
}

void Editor::insertText(const std::vector<uint32_t>& code_points) {
  text_.replace(selection_.toRange(), code_points);
  selection_.move(code_points.size(), text_.code_points().size());
  text_.dump();
}

bool Editor::performEditorCommands(const KeyEvent& key) {
  if (key.keysym == KeySymbol::KEY_V || key.keysym == KeySymbol::KEY_v) {
    requestClipboardText(
        [=](const std::string& text) { insertText(utf8ToCodePoints(text)); });
  }
  return true;
}
