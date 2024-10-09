#include "game.h"
#include "lighting/light.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static gfx_shader PRIMARY_SHADER;
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
  PRIMARY_SHADER = state.render.shader;
  setup_lights_texture();
  setup_lights_shader();
  add_light((vec2s){ 0.1f, 0.5f }, 1.0f, 400.0f);
  /*clear_lights();*/
  tex = gfx_load_texture_asset("creature_spawner", "png");
  font = gfx_load_font_asset("rubik", "ttf", 24);
  original_font_size = font.font_size;
  int res = gfx_loop_manager(window, false);
  return res;
}

bool gfx_loop(float delta) {
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  gfx_internal_renderer_set_shader(PRIMARY_SHADER);
  gfx_image_no_block(400, 400, tex, 0, 0, 0, 0, 1, true);
  state.current_div.scrollable = false;
  return true;
}

bool gfx_loop_post(float delta) {
  gfx_begin();
  gfx_internal_renderer_set_shader(LIGHTING_SHADER);
  render_lights();
  state.current_div.scrollable = false;
  return true;
};

bool gfx_loop_ui(float delta) {
  gfx_begin();
  gfx_internal_renderer_set_shader(PRIMARY_SHADER);
  update_font();
  gfx_push_font(&font);
  gfx_text("Test the Font");
  gfx_pop_font();
  state.current_div.scrollable = false;
  return true;
};
