#include "game.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/ecs_manager.h"
#include "physics/physics_setup.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>

comp_physics pc;
comp_renderer rc;
physics_info pi1, pi2;
renderer_info ri1, ri2;

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
  printf("OS: %s\n", OS_NAME);
  const GLubyte* vendor = glGetString(GL_VENDOR);
  const GLubyte* version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  } 
  cpVect gravity = cpv(0, 60);
  cpSpace *space = physics_setup(&gravity);
  ecs_setup(space, &pc, &rc, &pi1, &pi2, &ri1, &ri2);
  int res = gfx_loop_manager(window);
  ecs_cleanup(space, &pc, &rc, &pi1, &pi2, &ri1, &ri2);
  return res;
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  gfx_begin();
  ecs_update_system(ECS, PHYSICS_SYSTEM.id, 1.0f / 60.0f);
  ecs_update_system(ECS, RENDERER_SYSTEM.id, 1.0f / 60.0f);
}

