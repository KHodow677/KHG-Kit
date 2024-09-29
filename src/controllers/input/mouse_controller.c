#include "controllers/input/mouse_controller.h"
#include "data_utl/camera_utl.h"
#include "khg_gfx/events.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "menus/game_menu_manager.h"

static phy_vect handle_left_mouse_controls() {
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
    float screen_x = gfx_get_mouse_x();
    float screen_y = gfx_get_mouse_y();
    if (point_on_overlay(screen_x, screen_y)) {
      return phy_v(-1.0f, -1.0f);
    }
    return screen_to_world(screen_x, screen_y);
  }
  return phy_v(-1.0f, -1.0f);
}

static phy_vect handle_right_mouse_controls(void) {
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_RIGHT)) {
    float screen_x = gfx_get_mouse_x();
    float screen_y = gfx_get_mouse_y();
    if (point_on_overlay(screen_x, screen_y)) {
      return phy_v(-1.0f, -1.0f);
    }
    return screen_to_world(screen_x, screen_y);
  }
  return phy_v(-1.0f, -1.0f);
}

void update_mouse_controls(mouse_state *ms) {
  ms->left_mouse_click_controls = handle_left_mouse_controls();
  ms->right_mouse_click_controls = handle_right_mouse_controls();
}

