#include "khg_ui/ui.h"
#include "khg_ui/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdio.h>

int ui_testing() {
  glfwInit();
  GLFWwindow *window = glfwCreateWindow(800, 600, "Hello", NULL, NULL);
  glfwMakeContextCurrent(window);
  ui_init_glfw(800, 600, window);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    ui_begin();
    ui_text("Hello!");
    ui_end();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  ui_terminate();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

int main(int argc, char **argv) {
  return ui_testing();
  //return run_game();
}

