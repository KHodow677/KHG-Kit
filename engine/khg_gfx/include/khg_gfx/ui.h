#pragma once

#include "GLFW/glfw3.h"
#include <stdbool.h>
#include <stdint.h>

#define gfx_primary_item_color (gfx_color){ 133, 138, 148, 255 }
#define gfx_secondary_item_color (gfx_color){ 96, 100, 107, 255 }

#define gfx_no_color (gfx_color){0, 0, 0, 0}
#define gfx_white (gfx_color){ 255, 255, 255, 255 }
#define gfx_black (gfx_color){ 0, 0, 0, 255 }
#define gfx_red (gfx_color){ 255, 0, 0, 255 }
#define gfx_green (gfx_color){ 0, 255, 0, 255 }
#define gfx_blue (gfx_color){ 0, 0, 255, 255 }

void gfx_init_glfw(uint32_t display_width, uint32_t display_height, void *glfw_window);
int gfx_loop_manager(GLFWwindow *window, bool show_fps);
bool gfx_loop(float delta);
bool gfx_loop_post(float delta);
bool gfx_loop_ui(float delta);
void gfx_terminate(void);

