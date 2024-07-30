#include "chat.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

float ang = 0.0f;
gfx_texture body, top;

int gfx_start() {
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
  body = gfx_load_texture_asset("Tank-Body-Blue", "png");
  top = gfx_load_texture_asset("Tank-Top-Blue", "png");
  return gfx_loop_manager(window);
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  gfx_begin();
  ang += 0.05f;
  //gfx_rect(100, 50, gfx_white, 0.0f, ang);
  body.angle = ang;
  gfx_image(body);
  //gfx_image(top);
  gfx_text("Hello!");
}

int main(int argc, char *argv[]) {
  return gfx_start();
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
