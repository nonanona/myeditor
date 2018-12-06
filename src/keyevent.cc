#include "keyevent.h"

KeyEvent::KeyEvent(NativeKeyEvent* key_event)
    : key_event(key_event), keysym(static_cast<KeySymbol>(key_event->keyval)) {}

bool KeyEvent::isModifier(Modifier key) const {
  guint interest_bits = GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_META_MASK;
  guint request_bits = 0;
  switch (key) {
    case Modifier::CTRL:
      request_bits |= GDK_CONTROL_MASK;
      break;
    case Modifier::SHIFT:
      request_bits |= GDK_SHIFT_MASK;
      break;
    case Modifier::ALT:
      request_bits |= GDK_META_MASK;
      break;
  }
  return (key_event->state & interest_bits) == request_bits;
}

bool KeyEvent::hasModifier() const {
  guint interest_bits = GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_META_MASK;
  return (key_event->state & interest_bits) != 0;
}
