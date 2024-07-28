#include "chat.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int gfx_testing() {
  glfwInit();
  GLFWwindow *window = glfwCreateWindow(800, 600, "Hello", NULL, NULL);
  glfwMakeContextCurrent(window);
  gfx_init_glfw(800, 600, window);
  float ang = 0.0f;
  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    gfx_begin();
    ang += 0.05f;
    //gfx_rect(100, 50, gfx_white, 0.0f, ang);
    gfx_text("Hello!");
    gfx_end();
    glfwSwapBuffers(window);
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
    return EXIT_FAILURE;
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
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
