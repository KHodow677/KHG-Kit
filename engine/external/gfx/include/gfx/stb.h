#pragma once
#include "gfx/gfx.h"
#include <stdint.h>

int32_t get_max_char_height_font(LfFont font);

LfFont load_font(const char *filepath, uint32_t pixelsize, uint32_t tex_width, uint32_t tex_height,  uint32_t line_gap_add);
LfFont get_current_font(); 

LfTexture lf_load_texture(const char *filepath, bool flip, LfTextureFiltering filter);
LfTexture lf_load_texture_resized(const char *filepath, bool flip, LfTextureFiltering filter, uint32_t w, uint32_t h);
LfTexture lf_load_texture_resized_factor(const char *filepath, bool flip, LfTextureFiltering filter, float wfactor, float hfactor);
LfTexture lf_load_texture_from_memory(const void *data, size_t size, bool flip, LfTextureFiltering filter);
LfTexture lf_load_texture_from_memory_resized_factor(const void *data, size_t size, bool flip, LfTextureFiltering filter, float wfactor, float hfactor);
LfTexture lf_load_texture_from_memory_resized_to_fit(const void *data, size_t size, bool flip, LfTextureFiltering filter, int32_t container_w, int32_t container_h);

unsigned char *lf_load_texture_data(const char *filepath, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *lf_load_texture_data_resized(const char *filepath, int32_t w, int32_t h, int32_t *channels, bool flip);
unsigned char *lf_load_texture_data_resized_factor(const char *filepath, int32_t wfactor, int32_t hfactor, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *lf_load_texture_data_from_memory(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *lf_load_texture_data_from_memory_resized(const void *data, size_t size, int32_t *channels, int32_t *o_w, int32_t *o_h, bool flip, uint32_t w, uint32_t h);
unsigned char *lf_load_texture_data_from_memory_resized_to_fit_ex(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t i_channels, int32_t i_width, int32_t i_height, bool flip, int32_t container_w, int32_t container_h);
unsigned char *lf_load_texture_data_from_memory_resized_to_fit(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t *o_channels, bool flip, int32_t container_w, int32_t container_h);
unsigned char *lf_load_texture_data_from_memory_resized_factor(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip, float wfactor, float hfactor);

LfTextProps lf_text_render_wchar(vec2s pos, const wchar_t *str, LfFont font, LfColor color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index);
