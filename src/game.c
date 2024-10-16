#include "game.h"
#include "camera/camera.h"
#include "ecs/ecs_manager.h"
#include "khg_phy/vect.h"
#include "letterbox.h"
#include "lighting/light.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "physics/physics.h"
#include "scene/scene_manager.h"
#include "scene/scene_utl.h"
#include "thread/thread_manager.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static gfx_texture tex;
static gfx_font font;
static uint32_t original_font_size;

void log_sys_info() {
  printf("OS: %s\n", OS_NAME);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  }
}

static void update_font() {
  uint32_t min_change = fminf((uint32_t)(gfx_get_display_width() / 1280.0f * original_font_size), (uint32_t)(gfx_get_display_height() / 720.0f * original_font_size));
  if (font.font_size != min_change) {
    gfx_free_font(&font);
    font = gfx_load_font_asset("rubik", "ttf", min_change);
  }
}

static void render_div(float pos_x, float pos_y, float div_width, float div_height, float padding) {
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 0.0f;
  div_props.color = gfx_white;
  gfx_push_style_props(div_props);
  gfx_div_begin(((vec2s){ pos_x, pos_y }), ((vec2s){ div_width, div_height }), false);
}

int game_run() {
  if (!glfwInit()) {
    return -1;
  }
  GLFWwindow *window = glfwCreateWindow(1280, 720, "Game", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gfx_init_glfw(1280, 720, window);
  log_sys_info();
  worker_threads_setup();
  scenes_setup();
  scenes_switch(TO_MAIN_SCENE);
  tex = gfx_load_texture_asset("main/ground", "png");
  font = gfx_load_font_asset("rubik", "ttf", 24);
  original_font_size = font.font_size;
  int res = gfx_loop_manager(window, false);
  ecs_cleanup();
  physics_cleanup();
  worker_threads_cleanup();
  return res;
}

bool gfx_loop(float delta) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  if (check_current_scene("MAIN")) {
    gfx_clear_style_props();
    gfx_aabb letterbox = get_letterbox();
    render_div(letterbox.pos.x, letterbox.pos.y, letterbox.size.x, letterbox.size.y, 0);
    int div_width = 0;
    int div_height = 0;
    gfx_internal_renderer_set_shader(PRIMARY_SHADER);
    gfx_image_no_block(gfx_get_current_div().aabb.size.x / 2.0f, gfx_get_current_div().aabb.size.y, tex, 0, 0, 0, 0, 1, true);
    gfx_div_end();
    state.current_div.scrollable = false;
  }
  return true;
}

bool gfx_loop_post(float delta) {
  gfx_begin();
  gfx_clear_style_props();
  /*gfx_internal_renderer_set_shader(LIGHTING_SHADER);*/
  /*render_lights();*/
  /*state.current_div.scrollable = false;*/
  return true;
};

bool gfx_loop_ui(float delta) {
  gfx_begin();
  gfx_clear_style_props();
  gfx_internal_renderer_set_shader(PRIMARY_SHADER);
  update_font();
  gfx_push_font(&font);
  gfx_text("Test the Font");
  gfx_pop_font();
  state.current_div.scrollable = false;
  return true;
};
