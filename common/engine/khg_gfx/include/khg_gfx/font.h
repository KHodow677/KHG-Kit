#pragma once

#include "cglm/types-struct.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"

gfx_text_props gfx_text_render_char(vec2s pos, const char *str, gfx_font font, gfx_color color, int wrap_point, vec2s stop_point, bool no_render, bool render_solid, int start_index, int end_index);
gfx_font gfx_internal_load_font(const char *filepath, unsigned int pixelsize, unsigned int tex_width, unsigned int tex_height, unsigned int line_gap_add);
int get_max_char_height_font(gfx_font font);

