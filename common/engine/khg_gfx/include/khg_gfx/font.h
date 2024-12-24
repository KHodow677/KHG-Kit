#pragma once

#include "cglm/types-struct.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include <wchar.h>

gfx_text_props gfx_text_render_wchar(vec2s pos, const wchar_t *str, gfx_font font, gfx_color color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index);
gfx_font gfx_internal_load_font(const char *filepath, uint32_t pixelsize, uint32_t tex_width, uint32_t tex_height,  uint32_t line_gap_add);
int32_t get_max_char_height_font(gfx_font font);

