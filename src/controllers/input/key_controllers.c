#include "controllers/input/key_controllers.h"
#include "GLFW/glfw3.h"
#include "khg_gfx/events.h"

bool handle_key_button_is_down(int key) {
  return gfx_key_is_down(key);
}

bool handle_key_button_went_down(int key) {
  return gfx_key_went_down(key);
}

void update_key_controls(keyboard_state *ks) {
  ks->space_key_is_down = handle_key_button_is_down(GLFW_KEY_SPACE);
  ks->space_key_went_down = handle_key_button_went_down(GLFW_KEY_SPACE);
  ks->escape_key_is_down = handle_key_button_is_down(GLFW_KEY_ESCAPE);
  ks->escape_key_went_down = handle_key_button_went_down(GLFW_KEY_ESCAPE);
}

