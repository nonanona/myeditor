#pragma once

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <stdint.h>

using NativeKeyEvent = GdkEventKey;

enum class KeySymbol : uint32_t {

  // Alphabet keys
  KEY_A = GDK_KEY_A,
  KEY_B = GDK_KEY_B,
  KEY_C = GDK_KEY_C,
  KEY_D = GDK_KEY_D,
  KEY_E = GDK_KEY_E,
  KEY_F = GDK_KEY_F,
  KEY_G = GDK_KEY_G,
  KEY_H = GDK_KEY_H,
  KEY_I = GDK_KEY_I,
  KEY_J = GDK_KEY_J,
  KEY_K = GDK_KEY_K,
  KEY_L = GDK_KEY_L,
  KEY_M = GDK_KEY_M,
  KEY_N = GDK_KEY_N,
  KEY_O = GDK_KEY_O,
  KEY_P = GDK_KEY_P,
  KEY_Q = GDK_KEY_Q,
  KEY_R = GDK_KEY_R,
  KEY_S = GDK_KEY_S,
  KEY_T = GDK_KEY_T,
  KEY_U = GDK_KEY_U,
  KEY_V = GDK_KEY_V,
  KEY_W = GDK_KEY_W,
  KEY_X = GDK_KEY_X,
  KEY_Y = GDK_KEY_Y,
  KEY_Z = GDK_KEY_Z,

  KEY_a = GDK_KEY_a,
  KEY_b = GDK_KEY_b,
  KEY_c = GDK_KEY_c,
  KEY_d = GDK_KEY_d,
  KEY_e = GDK_KEY_e,
  KEY_f = GDK_KEY_f,
  KEY_g = GDK_KEY_g,
  KEY_h = GDK_KEY_h,
  KEY_i = GDK_KEY_i,
  KEY_j = GDK_KEY_j,
  KEY_k = GDK_KEY_k,
  KEY_l = GDK_KEY_l,
  KEY_m = GDK_KEY_m,
  KEY_n = GDK_KEY_n,
  KEY_o = GDK_KEY_o,
  KEY_p = GDK_KEY_p,
  KEY_q = GDK_KEY_q,
  KEY_r = GDK_KEY_r,
  KEY_s = GDK_KEY_s,
  KEY_t = GDK_KEY_t,
  KEY_u = GDK_KEY_u,
  KEY_v = GDK_KEY_v,
  KEY_w = GDK_KEY_w,
  KEY_x = GDK_KEY_x,
  KEY_y = GDK_KEY_y,
  KEY_z = GDK_KEY_z,

  // Cursor Keys
  KEY_LEFT = GDK_KEY_Left,
  KEY_RIGHT = GDK_KEY_Right,
  KEY_UP = GDK_KEY_Up,
  KEY_DOWN = GDK_KEY_Down,

  // Control Keys
  KEY_BACKSPACE = GDK_KEY_BackSpace,
  KEY_DELETE = GDK_KEY_Delete,

  KEY_ENTER = GDK_KEY_Return,
};

enum class Modifier : uint8_t {
  CTRL = 0,
  SHIFT,
  ALT,
};

inline bool isArrowKey(KeySymbol keysym) {
  return keysym == KeySymbol::KEY_LEFT || keysym == KeySymbol::KEY_RIGHT ||
         keysym == KeySymbol::KEY_UP || keysym == KeySymbol::KEY_DOWN;
}

struct KeyEvent {
  KeyEvent(NativeKeyEvent* key_event);

  NativeKeyEvent* key_event;
  KeySymbol keysym;

  bool isModifier(Modifier key) const;
  bool hasModifier() const;
};
