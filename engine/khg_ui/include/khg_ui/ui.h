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