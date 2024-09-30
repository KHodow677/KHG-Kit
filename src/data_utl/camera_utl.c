#include "data_utl/camera_utl.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"

phy_vect screen_to_world(float screen_x, float screen_y) {
  float window_center_x = gfx_get_display_width() / 2.0f;
  float window_center_y = gfx_get_display_height() / 2.0f;
  float world_x = (screen_x - window_center_x) / CAMERA.zoom + window_center_x + CAMERA.position.x;
  float world_y = (screen_y - window_center_y) / CAMERA.zoom + window_center_y + CAMERA.position.y;
  return phy_v(world_x, world_y);
}

phy_vect world_to_screen(float world_x, float world_y) {
  float window_center_x = gfx_get_display_width() / 2.0f;
  float window_center_y = gfx_get_display_height() / 2.0f;
  
  float screen_x = (world_x - CAMERA.position.x - window_center_x) * CAMERA.zoom + window_center_x;
  float screen_y = (world_y - CAMERA.position.y - window_center_y) * CAMERA.zoom + window_center_y;
  
  return phy_v(screen_x, screen_y);
}
