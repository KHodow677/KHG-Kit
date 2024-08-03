#include "game.h"
#include "entity/ecs_setup.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

float ang = 0.0f;
gfx_texture body, top;

int game_run() {
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
  ecs_setup();
  return gfx_loop_manager(window);
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  gfx_begin();
  ang += 0.05f;
  body.angle = ang;
  gfx_rect_no_block(300.0f, 250.0f, 145, 184, gfx_white, 0.0f, ang);
  gfx_image_no_block(200.0f, 150.0f, body);
  gfx_text("Hello!");
  gfx_image_no_block(200.0f, 150.0f, top);
}

