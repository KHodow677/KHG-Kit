#include "khg_gfx/events.h"
#include "khg_kin/namespace.h"
#include "util/camera/camera.h"
#include "util/io/cursor_controller.h"

cursor_state CURSOR_STATE = { 0 };

const bool cursor_button_is_down(unsigned int key) {
  return gfx_mouse_button_is_down(key);
}

const bool cursor_button_went_down(unsigned int key) {
  return gfx_mouse_button_went_down(key);
}

const bool cursor_button_went_up(unsigned int key) {
  return gfx_mouse_button_is_released(key);
}

void update_cursor_controls() {
  CURSOR_STATE.screen_pos.x = gfx_get_mouse_x();
  CURSOR_STATE.screen_pos.y = gfx_get_mouse_y();
  kin_vec pos = (kin_vec){ CURSOR_STATE.screen_pos.x, CURSOR_STATE.screen_pos.y };
  CURSOR_STATE.world_pos = screen_to_world(pos.x, pos.y);
}

