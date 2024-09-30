#include "game.h"
#include "game_manager.h"
#include "generators/components/map_generator.h"
#include "menus/title_menu.h"
#include "physics/physics_setup.h"
#include "scenes/scene_transition.h"
#include "scenes/scene_utl.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void log_sys_info() {
  printf("OS: %s\n", OS_NAME);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  }
}

int game_run() {
  if (!glfwInit()) {
    return -1;
  }
  GLFWwindow *window = glfwCreateWindow(WINDOW_START_WIDTH, WINDOW_START_HEIGHT, "It Tanks Two!", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gfx_init_glfw(WINDOW_START_WIDTH, WINDOW_START_HEIGHT, window);
  stm_init(&SCENE_FSM, &TITLE_SCENE, &TUTORIAL_SCENE);
  ecs_setup(false);
  load_map("Game-Floor", &GAME_FLOOR_MAP);
  load_map("Game-Building", &GAME_BUILDING_MAP);
  load_map("Game-Path", &GAME_PATH_MAP);
  int res = gfx_loop_manager(window, false);
  ecs_cleanup();
  free_map(&GAME_FLOOR_MAP);
  free_map(&GAME_BUILDING_MAP);
  free_map(&GAME_PATH_MAP);
  return res;
}

bool gfx_loop(float delta) {
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  if (check_current_scene("TITLE")) {
    gfx_clear_style_props();
    run_title();
    bool res = render_title_menu();
    gfx_rect_no_block(gfx_get_display_width() / 2.0f, gfx_get_display_height() / 2.0f, gfx_get_display_width(), gfx_get_display_height(), OVERLAY_FILTER_COLOR, 0.0f, 0.0f);
    return res;
  }
  else if (check_current_scene("TUTORIAL")) {
    return run_scene_gameplay(delta);
  }
  else if (check_current_scene("GAME")) {
    return run_scene_gameplay(delta);
  }
  return true;
}

