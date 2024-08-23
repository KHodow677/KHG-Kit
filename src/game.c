#include "game.h"
#include "controllers/elements/element_controller.h"
#include "game_manager.h"
#include "physics/physics_setup.h"
#include "spawners/spawn_tank.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "threading/thread_manager.h"
#include <stdlib.h>
#include <stdio.h>

void log_info() {
  printf("OS: %s\n", OS_NAME);
  const GLubyte* vendor = glGetString(GL_VENDOR);
  const GLubyte* version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  } 
}

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
  log_info();
  setup_worker_threads();
  SPACE = physics_setup(cpv(0.0f, 0.0f));
  ecs_setup();
  spawn_tank(600, 300);
  int res = gfx_loop_manager(window);
  ecs_cleanup();
  return res;
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  gfx_begin();
  ecs_update_system(ECS, MOVER_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, ROTATOR_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, SHOOTER_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, PHYSICS_SYSTEM.id, 0.0f);
  FOLLOWER_SYSTEM.current_degree = 1;
  ecs_update_system(ECS, FOLLOWER_SYSTEM.id, 0.0f);
  FOLLOWER_SYSTEM.current_degree = 2;
  ecs_update_system(ECS, FOLLOWER_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, ANIMATOR_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, RENDERER_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, DESTROYER_SYSTEM.id, 0.0f);
  cpSpaceStep(SPACE, 1.0f/60.0f);
}
