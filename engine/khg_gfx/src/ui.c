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
    utl_error_func("Trying to initialize gfx with GLFW without initializing GLFW first", utl_user_defined_data);
    return;
  }
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    utl_error_func("Failed to initialize Glad", utl_user_defined_data);
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
  gfx_internal_props_stack_create(&state.props_stack);
  memset(&state.grabbed_div, 0, sizeof(gfx_div));
  state.grabbed_div.id = -1;
  state.clipboard = clipboard_new(NULL);
  state.drawcalls = 0;
  glfwSetKeyCallback((GLFWwindow *)state.window_handle, gfx_internal_glfw_key_callback);
  glfwSetMouseButtonCallback((GLFWwindow *)state.window_handle, gfx_internal_glfw_mouse_button_callback);
  glfwSetScrollCallback((GLFWwindow *)state.window_handle, gfx_internal_glfw_scroll_callback);
  glfwSetCursorPosCallback((GLFWwindow *)state.window_handle, gfx_internal_glfw_cursor_callback);
  glfwSetCharCallback((GLFWwindow *)state.window_handle, gfx_internal_glfw_char_callback);
  glfwSetWindowSizeCallback((GLFWwindow *)state.window_handle, gfx_internal_glfw_window_size_callback);
  gfx_internal_renderer_init();
  state.tex_arrow_down = gfx_load_texture_asset("arrow-down", "png");
  state.tex_tick = gfx_load_texture_asset("tick", "png");
}

int gfx_loop_manager(GLFWwindow *window, bool show_fps) {
  double last_time = glfwGetTime();
  int frame_count = 0;
  double fps_start_time = last_time;
  while (!glfwWindowShouldClose(window)) {
    double current_time = glfwGetTime();
    double elapsed_time = current_time - last_time;
    last_time = current_time;
    gfx_loop(elapsed_time);
    frame_count++;
    double fps_elapsed_time = current_time - fps_start_time;
    if (fps_elapsed_time > 0.0) {
      if (show_fps) {
        double fps = frame_count / fps_elapsed_time;
        char title[256];
        snprintf(title, sizeof(title), "FPS: %.2f", fps);
        gfx_text(title);
      }
      frame_count = 0;
      fps_start_time = current_time;
    }
    gfx_end();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  gfx_terminate();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void gfx_terminate() {
  gfx_free_font(&state.theme.font);
}
