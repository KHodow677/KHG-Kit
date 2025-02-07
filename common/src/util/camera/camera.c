#include "GLFW/glfw3.h"
#include "game.h"
#include "khg_gfx/elements.h"
#include "util/camera/camera.h"
#include "khg_kin/namespace.h"
#include "util/letterbox.h"
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
  cam->position = (kin_vec){ -SCREEN_WIDTH / 2.0f, -SCREEN_HEIGHT / 2.0f };
  cam->target = (kin_vec){ 0.0f, 0.0f };
}

const kin_vec screen_to_world(const float screen_x, const float screen_y) {
  const float window_center_x = LETTERBOX.pos.x + LETTERBOX.size.x / 2.0f;
  const float window_center_y = LETTERBOX.pos.y + LETTERBOX.size.y / 2.0f;
  const float world_x = (screen_x - window_center_x) / CAMERA.zoom + window_center_x + (CAMERA.position.x * (gfx_get_display_width() / SCREEN_WIDTH));
  const float world_y = (screen_y - window_center_y) / CAMERA.zoom + window_center_y + (CAMERA.position.y * (gfx_get_display_height() / SCREEN_HEIGHT));
  return (kin_vec){ world_x, world_y };
}

const kin_vec world_to_screen(const float world_x, const float world_y) {
  const float window_center_x = LETTERBOX.pos.x + LETTERBOX.size.x / 2.0f;
  const float window_center_y = LETTERBOX.pos.y + LETTERBOX.size.y / 2.0f;
  const float screen_x = (world_x - CAMERA.position.x - window_center_x) * CAMERA.zoom + window_center_x;
  const float screen_y = (world_y - CAMERA.position.y - window_center_y) * CAMERA.zoom + window_center_y;
  return (kin_vec){ screen_x, screen_y };
}

const kin_vec world_to_screen_perc(const float world_x, const float world_y) {
  const kin_vec screen_pos = world_to_screen(world_x, world_y);
  return (kin_vec){ screen_pos.x / SCREEN_WIDTH, screen_pos.y / SCREEN_HEIGHT };
}

