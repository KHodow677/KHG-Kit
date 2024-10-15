#include "camera/camera.h"
#include "GLFW/glfw3.h"
#include "khg_gfx/elements.h"
#include "khg_phy/vect.h"
#include <stdio.h>

camera CAMERA = { 0 };
float INITIAL_ZOOM = 1.0f;

void camera_setup(camera *cam) {
  GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  printf("Screen width: %d\n", mode->width);
  printf("Screen height: %d\n", mode->height);
  INITIAL_ZOOM = mode->width / 1920.0f;
  cam->zoom = INITIAL_ZOOM;
  cam->target_zoom = INITIAL_ZOOM;
  cam->position = phy_v(-0.5f * gfx_get_current_div().aabb.size.x, -0.5f * gfx_get_current_div().aabb.size.y);
  cam->target = phy_v(-0.5f * gfx_get_current_div().aabb.size.x, -0.5f * gfx_get_current_div().aabb.size.y);
}

