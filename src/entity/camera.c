#include "entity/camera.h"
#include "GLFW/glfw3.h"
#include "khg_phy/vect.h"
#include <stdio.h>

float INITIAL_ZOOM = 1.0f;

void camera_setup(camera *cam) {
  GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  printf("Screen width: %d\n", mode->width);
  printf("Screen height: %d\n", mode->height);
  INITIAL_ZOOM = mode->width / 1920.0f;
  cam->position = phy_v(0.0f, 0.0f);
  cam->target = phy_v(0.0f, 0.0f);
  cam->zoom = INITIAL_ZOOM;
  cam->target_zoom = INITIAL_ZOOM;
}

