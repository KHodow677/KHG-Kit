#include "game.h"
#include "controllers/elements/tank_body_controller.h"
#include "controllers/elements/tank_top_controller.h"
#include "controllers/input/key_controllers.h"
#include "controllers/input/mouse_controller.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/ecs_manager.h"
#include "generators/elements/particle_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "khg_ecs/ecs.h"
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
comp_destroyer dc;

tank_body tb;
tank_top tt;
particle p;

cpVect left_clicked_pos;
cpVect right_clicked_pos;

void countBodies(cpBody *body, void *count) {
    (*(int *)count)++;
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
  printf("OS: %s\n", OS_NAME);
  const GLubyte* vendor = glGetString(GL_VENDOR);
  const GLubyte* version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  } 
  cpVect gravity = cpv(0.0f, 0.0f);
  left_clicked_pos = cpv(-1.0f, -1.0f); 
  right_clicked_pos = cpv(-1.0f, -1.0f); 
  space = physics_setup(&gravity);
  ecs_setup(space, &pc, &rc, &fc, &dc, &tb, &tt);
  int res = gfx_loop_manager(window);
  ecs_cleanup(space, &tb, &tt);
  return res;
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  gfx_begin();
  if (!cpveql(handle_left_mouse_controls(), cpv(-1.0f, -1.0f))) {
    left_clicked_pos = handle_left_mouse_controls();
  }
  if (!cpveql(left_clicked_pos, cpv(-1.0f, -1.0f))) {
    tank_body_target_position(&tb, left_clicked_pos, 60.0f, 1.0f);
  }
  if (!cpveql(handle_right_mouse_controls(), cpv(-1.0f, -1.0f))) {
    printf("Spawn\n");
    generate_particle(&p, ECS, space);
    right_clicked_pos = handle_right_mouse_controls();
    tt.is_locked_on = false;
  }
  if (!cpveql(right_clicked_pos, cpv(-1.0f, -1.0f))) {
    tank_top_lock_on_position(&tt, right_clicked_pos, 1.0f);
  }
  ecs_update_system(ECS, PHYSICS_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, FOLLOWER_SYSTEM.id, 0.0f);
  ecs_update_system(ECS, RENDERER_SYSTEM.id, 0.0f);
  if (handle_escape_button()) {
    tt.destroyer_info.destroy_now = true;
    tb.destroyer_info.destroy_now = true;
    free_tank_top(&tt, space);
    free_tank_body(&tb, space);
  }
  ecs_update_system(ECS, DESTROYER_SYSTEM.id, 0.0f);
  cpSpaceStep(space, 1.0f/60.0f);
}

