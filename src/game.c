#include "game.h"
#include "entity/ecs_setup.h"
#include "physics/physics_setup.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdio.h>

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
  cpVect gravity = cpv(0, 15);
  cpSpace *space = physics_setup(&gravity);
  ecs_setup(space);
  int res = gfx_loop_manager(window);
  physics_free(space);
  return res;
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  gfx_begin();
  ang += 0.05f;
  body.angle = ang;
  gfx_rect_no_block(300.0f, 250.0f, 145, 184, gfx_white, 0.0f, ang);
  gfx_image_no_block(200.0f, 150.0f, body);
  gfx_text("Hello!");
  gfx_image_no_block(200.0f, 150.0f, top);
}

