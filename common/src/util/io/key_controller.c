#include "GLFW/glfw3.h"
#include "khg_gfx/events.h"
#include "khg_gfx/internal.h"
#include "util/io/key_controller.h"

bool KEYBOARD_STATE[GLFW_KEY_LAST] = { 0 };

const bool key_button_is_down(unsigned int key) {
  return GFX_STATE.input.keyboard.keys[key];
}

const bool key_button_went_down(unsigned int key) {
  return KEYBOARD_STATE[key] && GFX_STATE.input.keyboard.keys[key];
}

const bool key_button_went_up(unsigned int key) {
  return KEYBOARD_STATE[key] && !GFX_STATE.input.keyboard.keys[key];
}

void update_key_controls() {
  for (unsigned int i = 0; i < GLFW_KEY_LAST; i++) {
    KEYBOARD_STATE[i] = gfx_key_changed(i);
  }
}

