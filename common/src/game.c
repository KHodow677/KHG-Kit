#define NAMESPACE_ELEMENT_USE
#define NAMESPACE_KIN_USE
#define NAMESPACE_TASKING_USE

#include "element/namespace.h"
#include "game.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "khg_kin/namespace.h"
#include "khg_utl/random.h"
#include "tasking/namespace.h"
#include "util/camera/camera.h"
#include "util/camera/camera_controller.h"
#include "util/io/key_controller.h"
#include "util/io/cursor_controller.h"
#include "util/letterbox.h"
#include "util/light.h"
#include "util/physics.h"
#include "scene/scene_loader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

float SCREEN_WIDTH = INITIAL_WIDTH;
float SCREEN_HEIGHT = INITIAL_HEIGHT;
float SCREEN_SCALE = 1.0f;

static gfx_font font;
static unsigned int original_font_size;

static GLFWwindow *game_init() {
  glfwInit();
  GLFWwindow *window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "Game", NULL, NULL);
  glfwMakeContextCurrent(window);
  gfx_init_glfw(INITIAL_WIDTH, INITIAL_HEIGHT, window);
  return window;
}

static void update_font() {
  unsigned int min_change = fminf((unsigned int)(gfx_get_display_width() / SCREEN_WIDTH * original_font_size), (unsigned int)(gfx_get_display_height() / SCREEN_WIDTH * original_font_size));
  if (font.font_size != min_change) {
    gfx_free_font(&font);
    font = gfx_load_font_asset("res/assets/fonts/acme-regular.ttf", min_change);
  }
}

static void render_div(float pos_x, float pos_y, float div_width, float div_height, float padding, gfx_color color) {
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 0.0f;
  div_props.color = color;
  gfx_push_style_props(div_props);
  gfx_div_begin(((vec2s){ pos_x, pos_y }), ((vec2s){ div_width, div_height }), false);
}

void log_sys_info() {
  printf("OS: %s\n", OS_NAME);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  }
}

const int game_run() {
  GLFWwindow *window = game_init();
  log_sys_info();
  utl_random_seed_clock();
  NAMESPACE_TASKING()->initialize_thread_pool();
  NAMESPACE_TASKING()->populate_texture_data("res/assets/data/tex_defs.ini");
  NAMESPACE_TASKING()->populate_tile_data("res/assets/data/tile_defs.ini");
  unsigned int test = NAMESPACE_TASKING()->get_texture_id("EMPTY_TEXTURE");
  unsigned int test2 = NAMESPACE_TASKING()->get_tile_id("EMPTY_TILE");
  NAMESPACE_TASKING()->load_tile_data(&test2);
  font = gfx_load_font_asset("res/assets/fonts/acme-regular.ttf", 50);
  original_font_size = font.font_size;
  setup_lights_shader();
  setup_scenes();
  int res = gfx_loop_manager(window, true);
  clear_scenes();
  NAMESPACE_TASKING()->clear_texture_data();
  NAMESPACE_TASKING()->shutdown_thread_pool();
  return res;
}

const bool gfx_loop(const float delta, const float fps_val) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  gfx_clear_style_props();
  gfx_internal_renderer_set_shader(PRIMARY_SHADER);
  update_key_controls();
  update_cursor_controls();
  get_letterbox();
  render_div(LETTERBOX.pos.x, LETTERBOX.pos.y, LETTERBOX.size.x, LETTERBOX.size.y, 0, GFX_WHITE);
  gfx_rect_no_block(LETTERBOX.pos.x + LETTERBOX.size.x / 2.0f, LETTERBOX.pos.y + LETTERBOX.size.y / 2.0f, LETTERBOX.size.x, LETTERBOX.size.y, (gfx_color){ 23, 21, 35, 255 }, 0.0f, 0.0f);
  move_camera(&CAMERA, delta);
  ecs_update_system(NAMESPACE_ELEMENT()->ECS, NAMESPACE_ELEMENT()->TILE_INFO.system_signature, delta);
  ecs_update_system(NAMESPACE_ELEMENT()->ECS, NAMESPACE_ELEMENT()->PHYSICS_INFO.system_signature, delta);
  ecs_update_system(NAMESPACE_ELEMENT()->ECS, NAMESPACE_ELEMENT()->RENDER_INFO.system_signature, delta);
  ecs_update_system(NAMESPACE_ELEMENT()->ECS, NAMESPACE_ELEMENT()->LIGHT_INFO.system_signature, delta);
  NAMESPACE_KIN()->engine_step(ENGINE, delta);
  gfx_div_end();
  GFX_STATE.current_div.scrollable = false;
  return true;
}

const bool gfx_loop_post(const float delta, const float fps_val) {
  gfx_begin();
  gfx_clear_style_props();
  gfx_internal_renderer_set_shader(LIGHTING_SHADER);
  glUniform1i(glGetUniformLocation(LIGHTING_SHADER.id, "u_num_lights_active"), LIGHT_COUNT);
  get_letterbox();
  render_div(LETTERBOX.pos.x, LETTERBOX.pos.y, LETTERBOX.size.x, LETTERBOX.size.y, 0, GFX_BLACK);
  render_lights();
  gfx_div_end();
  GFX_STATE.current_div.scrollable = false;
  return true;
};

const bool gfx_loop_ui(const float delta, const float fps_val) {
  gfx_begin();
  gfx_clear_style_props();
  gfx_internal_renderer_set_shader(PRIMARY_SHADER);
  update_font();
  gfx_push_font(&font);
  char fps[16]; 
  snprintf(fps, sizeof(fps), "FPS: %.2f", fps_val);
  gfx_set_ptr_x(LETTERBOX.pos.x);
  gfx_set_ptr_y(LETTERBOX.pos.y);
  gfx_text(fps);
  gfx_pop_font();
  GFX_STATE.current_div.scrollable = false;
  return true;
};

void gfx_framebuffer(const GLuint vao, const GLuint texture) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(FRAMEBUFFER_SHADER.id);
  float timeValue = (float)glfwGetTime();
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_bloom_spread"), 1.0f);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_bloom_intensity"), 1.0f);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_time"), timeValue);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_brightness_decrease"), 0.15f);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_noise_intensity"), 0.12f);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_distortion_strength"), 0.0005f);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_framebuffer_texture"), 0);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

