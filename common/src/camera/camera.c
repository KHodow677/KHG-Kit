#include "camera/camera.h"
#include "GLFW/glfw3.h"
#include "game.h"
#include "khg_gfx/elements.h"
#include "khg_phy/core/phy_vector.h"
#include "letterbox.h"
#include <stdio.h>

camera CAMERA = { 0 };
float INITIAL_ZOOM = 1.0f;

void camera_setup(camera *cam) {
  GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  printf("Screen width: %d\n", mode->width);
  printf("Screen height: %d\n", mode->height);
  cam->zoom = INITIAL_ZOOM;
  cam->target_zoom = INITIAL_ZOOM;
  cam->position = phy_vector2_new(-SCREEN_WIDTH / 2.0f, -SCREEN_HEIGHT / 2.0f);
  cam->target = phy_vector2_new(0.0f, 0.0f);
}

const phy_vector2 screen_to_world(const float screen_x, const float screen_y) {
  const float window_center_x = LETTERBOX.pos.x + LETTERBOX.size.x / 2.0f;
  const float window_center_y = LETTERBOX.pos.y + LETTERBOX.size.y / 2.0f;
  const float world_x = (screen_x - window_center_x) / CAMERA.zoom + window_center_x + (CAMERA.position.x * (gfx_get_display_width() / SCREEN_WIDTH));
  const float world_y = (screen_y - window_center_y) / CAMERA.zoom + window_center_y + (CAMERA.position.y * (gfx_get_display_height() / SCREEN_HEIGHT));
  return phy_vector2_new(world_x, world_y);
}

const phy_vector2 world_to_screen(const float world_x, const float world_y) {
  const float window_center_x = LETTERBOX.pos.x + LETTERBOX.size.x / 2.0f;
  const float window_center_y = LETTERBOX.pos.y + LETTERBOX.size.y / 2.0f;
  const float screen_x = (world_x - CAMERA.position.x - window_center_x) * CAMERA.zoom + window_center_x;
  const float screen_y = (world_y - CAMERA.position.y - window_center_y) * CAMERA.zoom + window_center_y;
  return phy_vector2_new(screen_x, screen_y);
}

const phy_vector2 world_to_screen_perc(const float world_x, const float world_y) {
  const phy_vector2 screen_pos = world_to_screen(world_x, world_y);
  return phy_vector2_new(screen_pos.x / SCREEN_WIDTH, screen_pos.y / SCREEN_HEIGHT);
}

