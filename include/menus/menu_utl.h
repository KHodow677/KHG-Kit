#pragma once

#include "khg_gfx/texture.h"
#include <stdbool.h>

void render_div(float pos_x, float pos_y, float div_width, float div_height, float padding);
void render_header_text(float div_width, float div_height, const char *text, float padding);
void render_body_text(float div_width, float div_height, const char *text, float padding);
void render_text(gfx_font font, float center_x, float center_y, const char *text);
bool render_button(float div_width, float div_height, const char *btn_text, float padding);
void render_big_icon(float div_width, float div_height, int tex_id, float padding);
void render_small_icon_buttons(float div_width, float div_height, float padding, float h_padding);

