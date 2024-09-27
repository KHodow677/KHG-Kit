#include "controllers/input/mouse_controller.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/events.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "menus/game_menu_manager.h"

static phy_vect handle_left_mouse_controls() {
  float world_x, world_y;
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
    float world_x = gfx_get_mouse_x();
    float world_y = gfx_get_mouse_y();
    if (point_on_overlay(world_x, world_y)) {
      return phy_v(-1.0f, -1.0f);
    }
    float window_center_x = gfx_get_display_width() / 2.0f;
    float window_center_y = gfx_get_display_height() / 2.0f;
    world_x = (world_x - window_center_x) / CAMERA.zoom + window_center_x + CAMERA.position.x;
    world_y = (world_y - window_center_y) / CAMERA.zoom + window_center_y + CAMERA.position.y;
    return phy_v(world_x, world_y);
  }
  return phy_v(-1.0f, -1.0f);
}

static phy_vect handle_right_mouse_controls(void) {
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_RIGHT)) {
    float world_x = gfx_get_mouse_x();
    float world_y = gfx_get_mouse_y();
    if (point_on_overlay(world_x, world_y)) {
      return phy_v(-1.0f, -1.0f);
    }
    float window_center_x = gfx_get_display_width() / 2.0f;
    float window_center_y = gfx_get_display_height() / 2.0f;
    world_x = (world_x - window_center_x) / CAMERA.zoom + window_center_x + CAMERA.position.x;
    world_y = (world_y - window_center_y) / CAMERA.zoom + window_center_y + CAMERA.position.y;
    return phy_v(world_x, world_y);
  }
  return phy_v(-1.0f, -1.0f);
}

void update_mouse_controls(mouse_state *ms) {
  ms->left_mouse_click_controls = handle_left_mouse_controls();
  ms->right_mouse_click_controls = handle_right_mouse_controls();
}

