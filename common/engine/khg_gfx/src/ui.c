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

void gfx_init_glfw(unsigned int display_width, unsigned int display_height, void* glfw_window) {
  setlocale(LC_ALL, "");
  if(!glfwInit()) {
    utl_error_func("Trying to initialize gfx with GLFW without initializing GLFW first", utl_user_defined_data);
    return;
  }
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    utl_error_func("Failed to initialize Glad", utl_user_defined_data);
    return;
  }
  memset(&GFX_STATE, 0, sizeof(GFX_STATE));
  GFX_STATE.init = true;
  GFX_STATE.dsp_w = display_width;
  GFX_STATE.dsp_h = display_height;
  GFX_STATE.window_handle = glfw_window;
  GFX_STATE.input.mouse.first_mouse_press = true;
  GFX_STATE.render.tex_count = 0;
  GFX_STATE.pos_ptr = (vec2s){0, 0};
  GFX_STATE.image_color_stack = GFX_NO_COLOR;
  GFX_STATE.active_element_id = 0;
  GFX_STATE.text_wrap = false;
  GFX_STATE.line_overflow = true;
  GFX_STATE.theme = gfx_default_theme();
  GFX_STATE.renderer_render = true;
  GFX_STATE.drag_state = (gfx_drag_state){ false, { 0, 0 }, 0 };
  gfx_internal_props_stack_create(&GFX_STATE.props_stack);
  memset(&GFX_STATE.grabbed_div, 0, sizeof(gfx_div));
  GFX_STATE.grabbed_div.id = -1;
  GFX_STATE.clipboard = clipboard_new(NULL);
  GFX_STATE.drawcalls = 0;
  glfwSetKeyCallback((GLFWwindow *)GFX_STATE.window_handle, gfx_internal_glfw_key_callback);
  glfwSetMouseButtonCallback((GLFWwindow *)GFX_STATE.window_handle, gfx_internal_glfw_mouse_button_callback);
  glfwSetScrollCallback((GLFWwindow *)GFX_STATE.window_handle, gfx_internal_glfw_scroll_callback);
  glfwSetCursorPosCallback((GLFWwindow *)GFX_STATE.window_handle, gfx_internal_glfw_cursor_callback);
  glfwSetCharCallback((GLFWwindow *)GFX_STATE.window_handle, gfx_internal_glfw_char_callback);
  glfwSetWindowSizeCallback((GLFWwindow *)GFX_STATE.window_handle, gfx_internal_glfw_window_size_callback);
  gfx_internal_renderer_init();
  GFX_STATE.tex_arrow_down = gfx_load_texture_asset("res/assets/textures/arrow_down.png");
  GFX_STATE.tex_tick = gfx_load_texture_asset("res/assets/textures/tick.png");
}

const int gfx_loop_manager(GLFWwindow *window, const bool show_fps) {
  GLuint framebuffer, texture;
  GLuint vao, vbo, ebo;
  gfx_setup_framebuffer(&framebuffer, &texture, &vao, &vbo, &ebo);
  float last_time = glfwGetTime();
  int frame_count = 0;
  float fps_start_time = last_time;
  float fps = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    float current_time = glfwGetTime();
    float elapsed_time = current_time - last_time;
    last_time = current_time;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gfx_get_display_width(), gfx_get_display_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!gfx_loop(elapsed_time, fps)) {
      break;
    }
    gfx_end();
    if (!gfx_loop_post(elapsed_time, fps)) {
      break;
    }
    gfx_end();
    if (!gfx_loop_ui(elapsed_time, fps)) {
      break;
    }
    gfx_end();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gfx_framebuffer(vao, texture);
    frame_count++;
    float fps_elapsed_time = current_time - fps_start_time;
    if (fps_elapsed_time > 0.0) {
      if (show_fps) {
        fps = frame_count / fps_elapsed_time;
        char title[64];
        snprintf(title, sizeof(title), "FPS: %.2f", fps);
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
  gfx_free_font(&GFX_STATE.theme.font);
}

