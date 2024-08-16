#include "controllers/input/mouse_controller.h"
#include "GLFW/glfw3.h"
#include "khg_gfx/events.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"

phy_vect handle_left_mouse_controls() {
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
    return cpv(gfx_get_mouse_x(), gfx_get_mouse_y());
  }
  return cpv(-1.0f, -1.0f);
}

phy_vect handle_right_mouse_controls(void) {
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_RIGHT)) {
    return cpv(gfx_get_mouse_x(), gfx_get_mouse_y());
  }
  return cpv(-1.0f, -1.0f);
}

