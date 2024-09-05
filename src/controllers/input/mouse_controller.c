#include "controllers/input/mouse_controller.h"
#include "game_manager.h"
#include "khg_gfx/events.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"

static phy_vect handle_left_mouse_controls() {
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
    return phy_v(gfx_get_mouse_x() + CAMERA.position.x, gfx_get_mouse_y() + CAMERA.position.y);
  }
  return phy_v(-1.0f, -1.0f);
}

static phy_vect handle_right_mouse_controls(void) {
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_RIGHT)) {
    return phy_v(gfx_get_mouse_x(), gfx_get_mouse_y());
  }
  return phy_v(-1.0f, -1.0f);
}

void update_mouse_controls(mouse_state *ms) {
  ms->left_mouse_click_controls = handle_left_mouse_controls();
  ms->right_mouse_click_controls = handle_right_mouse_controls();
}
