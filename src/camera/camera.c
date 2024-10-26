#include "camera/camera.h"
#include "GLFW/glfw3.h"
#include "game.h"
#include "khg_gfx/elements.h"
#include "khg_phy/vect.h"
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
  cam->position = phy_v(0.0f, 0.0f);
  cam->target = phy_v(0.0f, 0.0f);
}

phy_vect screen_to_world(float screen_x, float screen_y) {
  float window_center_x = LETTERBOX.pos.x + LETTERBOX.size.y / 2.0f;
  float window_center_y = LETTERBOX.pos.y + LETTERBOX.size.y / 2.0f;
  float world_x = (screen_x - window_center_x) / CAMERA.zoom + window_center_x + CAMERA.position.x;
  float world_y = (screen_y - window_center_y) / CAMERA.zoom + window_center_y + CAMERA.position.y;
  return phy_v(world_x, world_y);
}

phy_vect world_to_screen(float world_x, float world_y) {
  float window_center_x = LETTERBOX.pos.x + LETTERBOX.size.y / 2.0f;
  float window_center_y = LETTERBOX.pos.y + LETTERBOX.size.y / 2.0f;
  float screen_x = (world_x - CAMERA.position.x - window_center_x) * CAMERA.zoom + window_center_x;
  float screen_y = (world_y - CAMERA.position.y - window_center_y) * CAMERA.zoom + window_center_y;
  return phy_v(screen_x, screen_y);
}

phy_vect world_to_screen_perc(float world_x, float world_y) {
  phy_vect screen_pos = world_to_screen(world_x, world_y);
  return phy_v(screen_pos.x / INITIAL_WIDTH, screen_pos.y / INITIAL_HEIGHT);
}

