#include "gfx_test.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/texture.h"
#include "GLFW/glfw3.h"

float test_ang = 0.0f;
gfx_texture test_body, test_top;

int gfx_test() {
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
  test_body = gfx_load_texture_asset("Tank-Body-Blue", "png");
  test_top = gfx_load_texture_asset("Tank-Top-Blue", "png");
  return gfx_loop_manager(window);
}

#ifdef TESTING_GFX

#include "khg_gfx/elements.h"
#include "glad/glad.h"

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  gfx_begin();
  test_ang += 0.05f;
  test_body.angle = test_ang;
  gfx_rect_no_block(300.0f, 250.0f, 145, 184, gfx_white, 0.0f, test_ang);
  gfx_image_no_block(200.0f, 150.0f, test_body);
  gfx_text("Hello!");
  gfx_image_no_block(200.0f, 150.0f, test_top);
}

#endif

