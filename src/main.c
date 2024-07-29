#include "chat.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int gfx_testing() {
  if (!glfwInit()) {
    return -1;
  }
  GLFWwindow *window = glfwCreateWindow(800, 600, "Hello", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gfx_init_glfw(800, 600, window);
  float ang = 0.0f;
  const double target_fps = 60.0;
  const double frame_duration = 1.0 / target_fps;
  double last_time = glfwGetTime();
  double elapsed_time;
  while (!glfwWindowShouldClose(window)) {
    double current_time = glfwGetTime();
    elapsed_time = current_time - last_time;
    if (elapsed_time >= frame_duration) {
        last_time = current_time;
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        gfx_begin();
        ang += 0.05f;
        //gfx_rect(100, 50, gfx_white, 0.0f, ang);
        gfx_text("Hello!");
        gfx_end();
        glfwSwapBuffers(window);
        double frame_end_time = glfwGetTime();
        double frame_render_duration = frame_end_time - current_time;
        if (frame_render_duration < frame_duration) {
          double sleep_duration = frame_duration - frame_render_duration;
          glfwWaitEventsTimeout(sleep_duration);
        }
    }
    glfwPollEvents();
  }
  gfx_terminate();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

int main(int argc, char *argv[]) {
  return gfx_testing();
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <server|client>\n", argv[0]);
    return -1;
  }
  if (strcmp(argv[1], "server") == 0) {
    printf("Starting chat server...\n");
    return server_test();
  } 
  else if (strcmp(argv[1], "client") == 0) {
    printf("Starting chat client...\n");
    return client_test();
  } 
  else {
    fprintf(stderr, "Unknown argument '%s'. Use 'server' or 'client'.\n", argv[1]);
    return -1;
  }
  return 0;
}
