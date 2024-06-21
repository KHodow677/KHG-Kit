#include "khgenv/other.h"
#include "khgmath/minmax.h"
#include "GLFW/glfw3.h"

GLFWmonitor *get_current_monitor(GLFWwindow *window) {
  int n_monitors;
  int wx, wy, ww, wh;
  int mx, my, mw, mh;
  int overlap, best_overlap = 0;
  GLFWmonitor *best_monitor = NULL;
  const GLFWvidmode *mode;
  glfwGetWindowPos(window, &wx, &wy);
  glfwGetWindowSize(window, &ww, &wh);
  GLFWmonitor **monitors = glfwGetMonitors(&n_monitors);
  for (int i = 0; i < n_monitors; i++) {
    mode = glfwGetVideoMode(monitors[i]);
    glfwGetMonitorPos(monitors[i], &mx, &my);
    mw = mode->width;
    mh = mode->height;
    overlap = max(0, min(wx + ww, mx + mw) - max(wx, mx)) * max(0, min(wy + wh, my + mh) - max(wy, my));
    if (best_overlap < overlap) {
      best_overlap = overlap;
      best_monitor = monitors[i];
    }
  }
  return best_monitor;
}
