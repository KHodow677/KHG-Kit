#pragma once
#include "khg_ui/elements.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <wchar.h>

#define ui_primary_item_color (ui_color){ 133, 138, 148, 255 }
#define ui_secondary_item_color (ui_color){ 96, 100, 107, 255 }

#define ui_no_color (ui_color){0, 0, 0, 0}
#define ui_white (ui_color){ 255, 255, 255, 255 }
#define ui_black (ui_color){ 0, 0, 0, 255 }
#define ui_red (ui_color){ 255, 0, 0, 255 }
#define ui_green (ui_color){ 0, 255, 0, 255 }
#define ui_blue (ui_color){ 0, 0, 255, 255 }

void ui_init_glfw(uint32_t display_width, uint32_t display_height, void *glfw_window);
void ui_terminate();
ui_theme ui_default_theme();
ui_theme ui_get_theme();
void ui_set_theme(ui_theme theme);
void ui_resize_display(uint32_t display_width, uint32_t display_height);

#define ui_begin() ui_begin_loc(__FILE__, __LINE__)
void ui_begin_loc(const char* file, int32_t line);
void ui_end();
void ui_next_line();