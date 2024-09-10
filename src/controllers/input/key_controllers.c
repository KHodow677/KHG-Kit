#include "controllers/input/key_controllers.h"
#include "khg_gfx/events.h"

bool handle_key_button_is_down(int key) {
  bool ret = gfx_key_is_down(key);
  return ret;
}

bool handle_key_button_went_down(int key) {
  bool ret = gfx_key_went_down(key);
  return ret;
}

