#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "khg2d/texture.h"
#include "khg2d/renderer2d.h"
#include "khg2d/utils.h"

int main(int argc, char **argv) {
  GLFWwindow *window;
  renderer2d renderer;
  texture tex;
  glfwInit();
  window = glfwCreateWindow(840, 640, "Window", NULL, NULL);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress));
  init();
  createRenderer2d(&renderer, 0, 1000);
  loadFromFile(&tex, "./data/assets/images/test.jpg", KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  while (!glfwWindowShouldClose(window)) {
    int w = 0; int h = 0;
    vec4 color = { 0.1f, 0.2f, 0.6f, 1.0f };
    vec4 transform1 = { 100, 250, 100 ,100 };
    vec4 transform2 = { 100, 100, 100 ,100 };
    vec2 origin = { 0.0f, 0.0f };
    vec4 colorOrangeData[4], colorWhiteData[4];
    colorOrangeData[0] = colorOrange;
    colorOrangeData[1] = colorOrange;
    colorOrangeData[2] = colorOrange;
    colorOrangeData[3] = colorOrange;
    colorWhiteData[0] = colorWhite;
    colorWhiteData[1] = colorWhite;
    colorWhiteData[2] = colorWhite;
    colorWhiteData[3] = colorWhite;
    glfwGetWindowSize(window, &w, &h);
    updateWindowMetrics(&renderer, w, h);
    clearScreen(&renderer, color);
    renderRectangle(&renderer, transform1, colorOrangeData, origin, 0);
    renderRectangle(&renderer, transform2, colorOrangeData, origin, 0);
    flush(&renderer, true);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
