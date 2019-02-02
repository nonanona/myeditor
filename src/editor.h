#pragma once

#include "shaper.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "keyevent.h"
#include "selection.h"
#include "text_renderer.h"
#include "view.h"
#include "window.h"

class Editor {
 public:
  Editor(EGLDisplay display,
         EGLContext context,
         EGLSurface surface,
         IRect rect,
         Border border,
         Padding padding,
         FSize dpi);
  virtual ~Editor() {}

  void init(float text_size_pt);
  bool onKeyEvent(const KeyEvent& keyEvent);
  void onDraw();
  void onCursorDraw();
  void insertText(const std::vector<uint32_t>& code_points);

  bool performEditorCommands(const KeyEvent& key);

 private:
  Text text_;
  TextRenderer renderer_;
  FSize dpi_;
  Selection selection_;

  EditorWindow* window_;  // TODO: introduce WindowController
};
