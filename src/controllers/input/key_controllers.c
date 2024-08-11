#include "controllers/input/key_controllers.h"

#include "GLFW/glfw3.h"
#include "khg_gfx/events.h"

bool handle_escape_button() {
  return gfx_key_went_down(GLFW_KEY_ESCAPE);
}

