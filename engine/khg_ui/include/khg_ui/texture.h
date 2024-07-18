#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t id;
  uint32_t width, height;
} ui_texture;

typedef struct {
  void *cdata;
  void *font_info;
  uint32_t tex_width, tex_height;
  uint32_t line_gap_add, font_size;
  ui_texture bitmap;
  uint32_t num_glyphs;
} ui_font;

typedef enum {
  ui_tex_filter_linear,
  ui_tex_filter_nearest
} ui_texture_filtering;

typedef struct {
  float width, height;
  int32_t end_x, end_y;
  uint32_t rendered_count;
} ui_text_props;

ui_font ui_load_font(const char *filepath, uint32_t size);
ui_font ui_load_font_ex(const char *filepath, uint32_t size, uint32_t bitmap_w, uint32_t bitmap_h);

ui_texture ui_load_texture(const char *filepath, bool flip, ui_texture_filtering filter);
ui_texture ui_load_texture_resized(const char *filepath, bool flip, ui_texture_filtering filter, uint32_t w, uint32_t h);
ui_texture ui_load_texture_resized_factor(const char *filepath, bool flip, ui_texture_filtering filter, float wfactor, float hfactor);
ui_texture ui_load_texture_from_memory(const void *data, size_t size, bool flip, ui_texture_filtering filter);
ui_texture ui_load_texture_from_memory_resized(const void *data, size_t size, bool flip, ui_texture_filtering filter, uint32_t w, uint32_t h);
ui_texture ui_load_texture_from_memory_resized_factor(const void *data, size_t size, bool flip, ui_texture_filtering filter, float wfactor, float hfactor);
ui_texture ui_load_texture_from_memory_resized_to_fit(const void *data, size_t size, bool flip, ui_texture_filtering filter, int32_t container_w, int32_t container_h);

unsigned char *ui_load_texture_data(const char *filepath, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *ui_load_texture_data_resized(const char *filepath, int32_t w, int32_t h, int32_t *channels, bool flip);
unsigned char *ui_load_texture_data_resized_factor(const char *filepath, int32_t wfactor, int32_t hfactor, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *ui_load_texture_data_from_memory(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *ui_load_texture_data_from_memory_resized(const void *data, size_t size, int32_t *channels, int32_t *o_w, int32_t *o_h, bool flip, uint32_t w, uint32_t h);
unsigned char *ui_load_texture_data_from_memory_resized_to_fit_ex(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t i_channels, int32_t i_width, int32_t i_height, bool flip, int32_t container_w, int32_t container_h);
unsigned char *ui_load_texture_data_from_memory_resized_to_fit(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t *o_channels, bool flip, int32_t container_w, int32_t container_h);
unsigned char *ui_load_texture_data_from_memory_resized_factor(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip, float wfactor, float hfactor);

void ui_create_texture_from_image_data(ui_texture_filtering filter, uint32_t *id, int32_t width, int32_t height, int32_t channels, unsigned char* data); 
void ui_free_texture(ui_texture *tex);
ui_texture ui_load_texture_asset(const char *asset_name, const char *file_extension); 

void ui_free_font(ui_font *font);
ui_font ui_load_font_asset(const char *asset_name, const char *file_extension, uint32_t font_size);

