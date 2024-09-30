#include "game.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void log_sys_info() {
  printf("OS: %s\n", OS_NAME);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  }
}

int game_run() {
  if (!glfwInit()) {
    return -1;
  }
  GLFWwindow *window = glfwCreateWindow(1280, 720, "It Tanks Two!", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gfx_init_glfw(1280, 720, window);
  int res = gfx_loop_manager(window, false);
  return res;
}

bool gfx_loop(float delta) {
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  return true;
}

