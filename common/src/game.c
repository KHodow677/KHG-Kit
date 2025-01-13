#include "game.h"
#include "glad/glad.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include <math.h>
#include <stdio.h>

float SCREEN_WIDTH = INITIAL_WIDTH;
float SCREEN_HEIGHT = INITIAL_HEIGHT;
float SCREEN_SCALE = 1.0f;

static gfx_font font;
static unsigned int original_font_size;

static GLFWwindow *game_init() {
  glfwInit();
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game", NULL, NULL);
  glfwMakeContextCurrent(window);
  gfx_init_glfw(SCREEN_WIDTH, SCREEN_HEIGHT, window);
  return window;
}

static void update_font() {
  unsigned int min_change = fminf((unsigned int)(gfx_get_display_width() / SCREEN_WIDTH * original_font_size), (unsigned int)(gfx_get_display_height() / SCREEN_WIDTH * original_font_size));
  if (font.font_size != min_change) {
    gfx_free_font(&font);
    font = gfx_load_font_asset("res/assets/fonts/acme-regular.ttf", min_change);
  }
}

static void render_div(float pos_x, float pos_y, float div_width, float div_height, float padding) {
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 0.0f;
  div_props.color = GFX_WHITE;
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
  font = gfx_load_font_asset("res/assets/fonts/acme-regular.ttf", 50);
  original_font_size = font.font_size;
  int res = gfx_loop_manager(window, false);
  return res;
}

const bool gfx_loop(const float delta) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  return true;
}

const bool gfx_loop_post(const float delta) {
  return true;
};

const bool gfx_loop_ui(const float delta) {
  return true;
};

void gfx_framebuffer(const GLuint vao, const GLuint texture) {
}

