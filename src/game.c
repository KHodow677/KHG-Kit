#include "game.h"
#include "camera/camera.h"
#include "letterbox.h"
#include "camera/camera_controller.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_mover.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/comp_zone.h"
#include "ecs/ecs_manager.h"
#include "io/key_controller.h"
#include "graphics/light.h"
#include "physics/physics.h"
#include "resources/area_loader.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "scene/scene_manager.h"
#include "scene/scene_utl.h"
#include "khg_phy/space.h"
#include "khg_ecs/ecs.h"
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

float SCREEN_WIDTH = INITIAL_WIDTH;
float SCREEN_HEIGHT = INITIAL_HEIGHT;
float SCREEN_SCALE = 1.0f;

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
  uint32_t min_change = fminf((uint32_t)(gfx_get_display_width() / SCREEN_WIDTH * original_font_size), (uint32_t)(gfx_get_display_height() / SCREEN_WIDTH * original_font_size));
  if (font.font_size != min_change) {
    gfx_free_font(&font);
    font = gfx_load_font_asset("res/assets/fonts/inter.ttf", min_change);
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

const int game_run() {
  if (!glfwInit()) {
    return -1;
  }
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gfx_init_glfw(SCREEN_WIDTH, SCREEN_HEIGHT, window);
  log_sys_info();
  generate_textures();
  generate_rigs();
  generate_areas();
  scenes_setup();
  scenes_switch(TO_MAIN_SCENE);
  font = gfx_load_font_asset("res/assets/fonts/inter.ttf", 50);
  original_font_size = font.font_size;
  int res = gfx_loop_manager(window, false);
  cleanup_areas();
  ecs_cleanup();
  physics_cleanup();
  return res;
}

const bool gfx_loop(const float delta) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  update_key_controls(&KEYBOARD_STATE);
  if (check_current_scene("MAIN")) {
    gfx_clear_style_props();
    get_letterbox();
    render_div(LETTERBOX.pos.x, LETTERBOX.pos.y, LETTERBOX.size.x, LETTERBOX.size.y, 0);
    gfx_internal_renderer_set_shader(PRIMARY_SHADER);
    move_camera(&CAMERA, delta);
    ecs_update_system(ECS, ANIMATOR_SYSTEM_SIGNATURE, delta);
    ecs_update_system(ECS, LIGHT_SYSTEM_SIGNATURE, delta);
    ecs_update_system(ECS, PHYSICS_SYSTEM_SIGNATURE, delta);
    ecs_update_system(ECS, RENDERER_SYSTEM_SIGNATURE, delta);
    ecs_update_system(ECS, ZONE_SYSTEM_SIGNATURE, delta);
    ecs_update_system(ECS, MOVER_SYSTEM_SIGNATURE, delta);
    phy_space_step(SPACE, delta);
    gfx_div_end();
    state.current_div.scrollable = false;
  }
  return true;
}

const bool gfx_loop_post(const float delta) {
  gfx_begin();
  gfx_clear_style_props();
  gfx_internal_renderer_set_shader(LIGHTING_SHADER);
  render_lights();
  state.current_div.scrollable = false;
  return true;
};

const bool gfx_loop_ui(const float delta) {
  gfx_begin();
  gfx_internal_renderer_set_shader(PRIMARY_SHADER);
  update_font();
  gfx_push_font(&font);
  gfx_text("Test the Font");
  gfx_pop_font();
  state.current_div.scrollable = false;
  return true;
};

void gfx_framebuffer(const GLuint vao, const GLuint texture) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(FRAMEBUFFER_SHADER.id);
  float timeValue = (float)glfwGetTime();
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "time"), timeValue);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "brightness_decrease"), 0.15f);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "noise_intensity"), 0.12f);
  glUniform1f(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "distortion_strength"), 0.0015f);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(FRAMEBUFFER_SHADER.id, "u_framebuffer_texture"), 0);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

