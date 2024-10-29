#include "io/key_controller.h"
#include "GLFW/glfw3.h"
#include "khg_gfx/events.h"

keyboard_state KEYBOARD_STATE = { 0 };

bool handle_key_button_is_down(int key) {
  return gfx_key_is_down(key);
}

bool handle_key_button_went_down(int key) {
  return gfx_key_went_down(key);
}

void update_key_controls(keyboard_state *ks) {
  ks->w_key_is_down = handle_key_button_is_down(GLFW_KEY_W);
  ks->w_key_went_down = handle_key_button_went_down(GLFW_KEY_W);
  ks->a_key_is_down = handle_key_button_is_down(GLFW_KEY_A);
  ks->a_key_went_down = handle_key_button_went_down(GLFW_KEY_A);
  ks->s_key_is_down = handle_key_button_is_down(GLFW_KEY_S);
  ks->s_key_went_down = handle_key_button_went_down(GLFW_KEY_S);
  ks->d_key_is_down = handle_key_button_is_down(GLFW_KEY_D);
  ks->d_key_went_down = handle_key_button_went_down(GLFW_KEY_D);
  ks->space_key_is_down = handle_key_button_is_down(GLFW_KEY_SPACE);
  ks->space_key_went_down = handle_key_button_went_down(GLFW_KEY_SPACE);
  ks->escape_key_is_down = handle_key_button_is_down(GLFW_KEY_ESCAPE);
  ks->escape_key_went_down = handle_key_button_went_down(GLFW_KEY_ESCAPE);
}

