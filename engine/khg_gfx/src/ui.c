#include "glad/glad.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/ui.h"
#include "khg_utl/error_func.h"
#include "libclipboard/libclipboard.h"
#include <locale.h>
#include <string.h>
#include <stdio.h>

void gfx_init_glfw(uint32_t display_width, uint32_t display_height, void* glfw_window) {
  setlocale(LC_ALL, "");
  if(!glfwInit()) {
    error_func("Trying to initialize Leif with GLFW without initializing GLFW first", user_defined_data);
    return;
  }
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    error_func("Failed to initialize Glad", user_defined_data);
    return;
  }
  memset(&state, 0, sizeof(state));
  state.init = true;
  state.dsp_w = display_width;
  state.dsp_h = display_height;
  state.window_handle = glfw_window;
  state.input.mouse.first_mouse_press = true;
  state.render.tex_count = 0;
  state.pos_ptr = (vec2s){0, 0};
  state.image_color_stack = gfx_no_color;
  state.active_element_id = 0;
  state.text_wrap = false;
  state.line_overflow = true;
  state.theme = gfx_default_theme();
  state.renderer_render = true;
  state.drag_state = (gfx_drag_state){ false, { 0, 0 }, 0 };
  props_stack_create(&state.props_stack);
  memset(&state.grabbed_div, 0, sizeof(gfx_div));
  state.grabbed_div.id = -1;
  state.clipboard = clipboard_new(NULL);
  state.drawcalls = 0;
  glfwSetKeyCallback((GLFWwindow *)state.window_handle, glfw_key_callback);
  glfwSetMouseButtonCallback((GLFWwindow *)state.window_handle, glfw_mouse_button_callback);
  glfwSetScrollCallback((GLFWwindow *)state.window_handle, glfw_scroll_callback);
  glfwSetCursorPosCallback((GLFWwindow *)state.window_handle, glfw_cursor_callback);
  glfwSetCharCallback((GLFWwindow *)state.window_handle, glfw_char_callback);
  glfwSetWindowSizeCallback((GLFWwindow *)state.window_handle, glfw_window_size_callback);
  renderer_init();
  state.tex_arrow_down = gfx_load_texture_asset("arrow-down", "png");
  state.tex_tick = gfx_load_texture_asset("tick", "png");
}

void gfx_terminate() {
  gfx_free_font(&state.theme.font);
}
