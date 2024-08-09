#include "game.h"
#include "controllers/elements/tank_body_controller.h"
#include "controllers/elements/tank_top_controller.h"
#include "controllers/input/mouse_controller.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/ecs_manager.h"
#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "physics/physics_setup.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>

cpSpace *space;

comp_physics pc;
comp_renderer rc;
comp_follower fc;

tank_body tb;
tank_top tt;

cpVect clicked_pos;

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
  cpVect gravity = cpv(0, 0);
  space = physics_setup(&gravity);
  tb = (tank_body){ 0 };
  tb.physics_info = (physics_info){ 0 };
  tb.renderer_info = (renderer_info){ 0 };
  tt = (tank_top){ 0 };
  tt.physics_info = (physics_info) { 0 };
  tt.renderer_info = (renderer_info) { 0 };
  tt.follower_info = (follower_info) { 0 };
  ecs_setup(space, &pc, &rc, &fc, &tb, &tt);
  int res = gfx_loop_manager(window);
  ecs_cleanup(space, &tb, &tt);
  return res;
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  gfx_begin();
  if (!tb.physics_info.is_moving && !tb.physics_info.is_turning) {
    clicked_pos = handle_mouse_controls();
  }
  if (!cpveql(clicked_pos, cpv(-1.0f, -1.0f)) && !tb.physics_info.is_moving) {
    tank_body_rotate_to_position(&tb, &tt, clicked_pos, 2);
  }
  if (!cpveql(clicked_pos, cpv(-1.0f, -1.0f)) && !tb.physics_info.is_turning) {
    tank_body_move_to_position(&tb, &tt, clicked_pos, 120);
  }
  tank_top_set_rotation_speed(&tt, 2);
  ecs_update_system(ECS, PHYSICS_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, FOLLOWER_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, RENDERER_SYSTEM.id, 0.0f);
  cpSpaceStep(space, 1.0f/60.0f);
}

