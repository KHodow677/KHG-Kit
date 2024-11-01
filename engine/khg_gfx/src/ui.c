#include "glad/glad.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/ui.h"
#include "khg_utl/error_func.h"
#include "libclipboard/libclipboard.h"
#include <locale.h>
#include <string.h>
#include <stdio.h>

static void gfx_setup_framebuffer(GLuint *framebuffer, GLuint *texture, GLuint *vao, GLuint *vbo, GLuint *ebo) {
  float framebuffer_quad_vertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  unsigned int quadIndices[] = {
    0, 1, 2,
    0, 2, 3
  };
  glGenVertexArrays(1, vao);
  glGenBuffers(1, vbo);
  glGenBuffers(1, ebo);
  glBindVertexArray(*vao);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(framebuffer_quad_vertices), framebuffer_quad_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glGenFramebuffers(1, framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gfx_get_display_width(), gfx_get_display_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    utl_error_func("Framebuffer is not complete", utl_user_defined_data);
  }
}

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
  state.tex_arrow_down = gfx_load_texture_asset("arrow_down", "png");
  state.tex_tick = gfx_load_texture_asset("tick", "png");
}

const int gfx_loop_manager(GLFWwindow *window, const bool show_fps) {
  GLuint framebuffer, texture;
  GLuint vao, vbo, ebo;
  gfx_setup_framebuffer(&framebuffer, &texture, &vao, &vbo, &ebo);
  double last_time = glfwGetTime();
  int frame_count = 0;
  double fps_start_time = last_time;
  while (!glfwWindowShouldClose(window)) {
    double current_time = glfwGetTime();
    double elapsed_time = current_time - last_time;
    last_time = current_time;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gfx_get_display_width(), gfx_get_display_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!gfx_loop(elapsed_time)) {
      break;
    }
    gfx_end();
    if (!gfx_loop_post(elapsed_time)) {
      break;
    }
    gfx_end();
    if (!gfx_loop_ui(elapsed_time)) {
      break;
    }
    gfx_end();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gfx_framebuffer(vao, texture);
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

