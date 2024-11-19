#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  uint32_t id;
  uint32_t width, height;
  float angle;
} gfx_texture;

typedef struct {
  void *cdata;
  void *font_info;
  uint8_t *buffer;
  uint32_t tex_width, tex_height;
  uint32_t line_gap_add, font_size;
  gfx_texture bitmap;
  uint32_t num_glyphs;
} gfx_font;

typedef enum {
  gfx_tex_filter_linear,
  gfx_tex_filter_nearest
} gfx_texture_filtering;

typedef struct {
  float width, height;
  int32_t end_x, end_y;
  uint32_t rendered_count;
} gfx_text_props;

gfx_font gfx_load_font(const char *filepath, uint32_t size);
gfx_font gfx_load_font_ex(const char *filepath, uint32_t size, uint32_t bitmap_w, uint32_t bitmap_h);

gfx_texture gfx_load_texture(const char *filepath, bool flip, gfx_texture_filtering filter);
gfx_texture gfx_load_texture_resized(const char *filepath, bool flip, gfx_texture_filtering filter, uint32_t w, uint32_t h);
gfx_texture gfx_load_texture_resized_factor(const char *filepath, bool flip, gfx_texture_filtering filter, float wfactor, float hfactor);
gfx_texture gfx_load_texture_from_memory(const void *data, size_t size, bool flip, gfx_texture_filtering filter);
gfx_texture gfx_load_texture_from_memory_resized(const void *data, size_t size, bool flip, gfx_texture_filtering filter, uint32_t w, uint32_t h);
gfx_texture gfx_load_texture_from_memory_resized_factor(const void *data, size_t size, bool flip, gfx_texture_filtering filter, float wfactor, float hfactor);
gfx_texture gfx_load_texture_from_memory_resized_to_fit(const void *data, size_t size, bool flip, gfx_texture_filtering filter, int32_t container_w, int32_t container_h);

unsigned char *gfx_load_texture_data(const char *filepath, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *gfx_load_texture_data_resized(const char *filepath, int32_t w, int32_t h, int32_t *channels, bool flip);
unsigned char *gfx_load_texture_data_resized_factor(const char *filepath, int32_t wfactor, int32_t hfactor, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *gfx_load_texture_data_from_memory(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip);
unsigned char *gfx_load_texture_data_from_memory_resized(const void *data, size_t size, int32_t *channels, int32_t *o_w, int32_t *o_h, bool flip, uint32_t w, uint32_t h);
unsigned char *gfx_load_texture_data_from_memory_resized_to_fit_ex(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t i_channels, int32_t i_width, int32_t i_height, bool flip, int32_t container_w, int32_t container_h);
unsigned char *gfx_load_texture_data_from_memory_resized_to_fit(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t *o_channels, bool flip, int32_t container_w, int32_t container_h);
unsigned char *gfx_load_texture_data_from_memory_resized_factor(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip, float wfactor, float hfactor);

void gfx_create_texture_from_image_data(gfx_texture_filtering filter, uint32_t *id, int32_t width, int32_t height, int32_t channels, unsigned char* data); 
void gfx_free_texture(gfx_texture *tex);
gfx_texture gfx_load_texture_asset(const char *filepath);

void gfx_free_font(gfx_font *font);
gfx_font gfx_load_font_asset(const char *asset_name, const char *file_extension, uint32_t font_size);

