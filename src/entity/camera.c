#include "entity/camera.h"
#include "GLFW/glfw3.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_phy/vect.h"
#include <stdio.h>

float INITIAL_ZOOM = 1.0f;

void camera_setup(camera *cam, bool real_game, float x, float y) {
  GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  printf("Screen width: %d\n", mode->width);
  printf("Screen height: %d\n", mode->height);
  INITIAL_ZOOM = mode->width / 1920.0f;
  cam->move_enabled = true;
  cam->zoom = INITIAL_ZOOM;
  cam->target_zoom = INITIAL_ZOOM;
  if (real_game) {
    float window_center_x = gfx_get_display_width() / 2.0f;
    float window_center_y = gfx_get_display_height() / 2.0f;
    float range_x = (GAME_MAP_TILE_SIZE * GAME_MAP_WIDTH * 0.5f) - (window_center_x / (window_center_x * cam->zoom)) * window_center_x + window_center_x;
    float range_y = (GAME_MAP_TILE_SIZE * GAME_MAP_HEIGHT * 0.5f) - (window_center_y / (window_center_y * cam->zoom)) * window_center_y + window_center_y;
    cam->position = phy_v(-0.5f * gfx_get_display_width(), range_y - gfx_get_display_height());
    cam->target = phy_v(-0.5f * gfx_get_display_width(), range_y - gfx_get_display_height());
  }
  else {
    cam->position = phy_v(x - gfx_get_display_width() * 0.5f, y - gfx_get_display_height() * 0.5f);
    cam->target = phy_v(x - gfx_get_display_width() * 0.5f, y - gfx_get_display_height() * 0.5f);
  }
}

