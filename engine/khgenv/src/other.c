#include "khgenv/other.h"
#include "khgmath/minmax.h"
#include "GLFW/glfw3.h"

GLFWmonitor *getCurrentMonitor(GLFWwindow *window) {
  int nmonitors, i;
  int wx, wy, ww, wh;
  int mx, my, mw, mh;
  int overlap, bestOverlap = 0;
  GLFWmonitor *bestMonitor = NULL;
  const GLFWvidmode *mode;
  glfwGetWindowPos(window, &wx, &wy);
  glfwGetWindowSize(window, &ww, &wh);
  GLFWmonitor **monitors = glfwGetMonitors(&nmonitors);
  for (i = 0; i < nmonitors; i++) {
    mode = glfwGetVideoMode(monitors[i]);
    glfwGetMonitorPos(monitors[i], &mx, &my);
    mw = mode->width;
    mh = mode->height;
    overlap = max(0, min(wx + ww, mx + mw) - max(wx, mx)) * max(0, min(wy + wh, my + mh) - max(wy, my));
    if (bestOverlap < overlap) {
      bestOverlap = overlap;
      bestMonitor = monitors[i];
    }
  }
  return bestMonitor;
}
