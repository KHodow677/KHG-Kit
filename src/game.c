#include "game.h"
#include "controllers/elements/camera_controller.h"
#include "controllers/input/key_controllers.h"
#include "controllers/input/mouse_controller.h"
#include "entity/map.h"
#include "game_manager.h"
#include "generators/components/map_generator.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_phy/threaded_space.h"
#include "khg_stm/state_machine.h"
#include "menus/title_menu.h"
#include "physics/physics_setup.h"
#include "scenes/scene_utl.h"
#include "spawners/spawn_tank.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "khg_phy/vect.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "threading/thread_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  stm_init(&SCENE_FSM, &TITLE_SCENE, &SANDBOX_SCENE);
  printf("Current Scene: %s\n", (char *)stm_current_state(&SCENE_FSM)->data);
  SPACE = physics_setup(phy_v(0.0f, 0.0f));
  LARGE_FONT = gfx_load_font_asset("Rubik-Bold", "ttf", 48);
  MEDIUM_FONT = gfx_load_font_asset("Rubik-Bold", "ttf", 32);
  ecs_setup();
  load_map("Map_Floor", &GAME_FLOOR_MAP);
  load_map("Map_Building", &GAME_BUILDING_MAP);
  spawn_tank(600, 300);
  spawn_tank(300, 300);
  int res = gfx_loop_manager(window, false);
  ecs_cleanup();
  return res;
}

bool gfx_loop(float delta) {
  glClear(GL_COLOR_BUFFER_BIT);
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  gfx_begin();
  if (check_current_scene("TITLE")) {
    gfx_clear_style_props();
    return render_title_menu();
  }
  else {
    gfx_clear_style_props();
    gfx_element_props game_props = gfx_get_theme().image_props;
    gfx_push_style_props(game_props);
    update_mouse_controls(&MOUSE_STATE);
    update_key_controls(&KEYBOARD_STATE);
    move_camera(&CAMERA, delta);
    render_map(GAME_FLOOR_MAP);
    render_map(GAME_BUILDING_MAP);
    ecs_update_system(ECS, SELECTOR_SYSTEM.id, delta);
    ecs_update_system(ECS, MOVER_SYSTEM.id, delta);
    ecs_update_system(ECS, ROTATOR_SYSTEM.id, delta);
    ecs_update_system(ECS, SHOOTER_SYSTEM.id, delta);
    ecs_update_system(ECS, PHYSICS_SYSTEM.id, delta);
    ecs_update_system(ECS, ANIMATOR_SYSTEM.id, delta);
    ecs_update_system(ECS, RENDERER_SYSTEM.id, delta);
    ecs_update_system(ECS, DESTROYER_SYSTEM.id, delta);
    phy_threaded_space_step(SPACE, delta);
    state.current_div.scrollable = false;
    gfx_pop_style_props();
    return true;
  }
}
