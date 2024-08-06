#include "game.h"
#include "entity/ecs_manager.h"
#include "physics/physics_setup.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdlib.h>
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
  printf("OS: %s\n", OS_NAME);
  const GLubyte* vendor = glGetString(GL_VENDOR);
  const GLubyte* version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  } 
  body = gfx_load_texture_asset("Tank-Body-Blue", "png");
  top = gfx_load_texture_asset("Tank-Top-Blue", "png");
  cpVect gravity = cpv(0, 60);
  cpSpace *space = physics_setup(&gravity);
  comp_physics physics_component;
  comp_renderer renderer_component;
  ecs_setup(space, &physics_component, &renderer_component);
  int res = gfx_loop_manager(window);
  ecs_cleanup(space, &physics_component, &renderer_component);
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

