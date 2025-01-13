#include "game.h"
#include "glad/glad.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
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

