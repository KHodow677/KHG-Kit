#pragma once

#include "GLFW/glfw3.h"
#include <stdbool.h>
#include <stdint.h>

#define GFX_PRIMARY_ITEM_COLOR (gfx_color){ 133, 138, 148, 255 }
#define GFX_SECONDARY_ITEM_COLOR (gfx_color){ 96, 100, 107, 255 }

#define GFX_NO_COLOR (gfx_color){0, 0, 0, 0}
#define GFX_WHITE (gfx_color){ 255, 255, 255, 255 }
#define GFX_BLACK (gfx_color){ 0, 0, 0, 255 }
#define GFX_RED (gfx_color){ 255, 0, 0, 255 }
#define GFX_GREEN (gfx_color){ 0, 255, 0, 255 }
#define GFX_BLUE (gfx_color){ 0, 0, 255, 255 }

void gfx_init_glfw(unsigned int display_width, unsigned int display_height, void *glfw_window);
const int gfx_loop_manager(GLFWwindow *window, const bool show_fps);
const bool gfx_loop(const float delta, const float fps_val);
const bool gfx_loop_post(const float delta, const float fps_val);
const bool gfx_loop_ui(const float delta, const float fps_val);
void gfx_framebuffer(const unsigned int vao, const unsigned int texture);
void gfx_terminate(void);

