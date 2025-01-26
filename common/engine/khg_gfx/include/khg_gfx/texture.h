#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  unsigned int id;
  unsigned int width, height;
  float angle;
} gfx_texture;

typedef struct {
  void *cdata;
  void *font_info;
  unsigned char *buffer;
  unsigned int tex_width, tex_height;
  unsigned int line_gap_add, font_size;
  gfx_texture bitmap;
  unsigned int num_glyphs;
} gfx_font;

typedef enum {
  GFX_TEX_FILTER_LINEAR,
  GFX_TEX_FILTER_NEAREST
} gfx_texture_filtering;

typedef struct {
  float width, height;
  int end_x, end_y;
  unsigned int rendered_count;
} gfx_text_props;

gfx_font gfx_load_font(const char *filepath, unsigned int size);
gfx_font gfx_load_font_ex(const char *filepath, unsigned int size, unsigned int bitmap_w, unsigned int bitmap_h);

gfx_texture gfx_load_texture(const char *filepath, bool flip, gfx_texture_filtering filter);
void gfx_fetch_texture_raw(unsigned char **image, const char *filepath, int *width, int *height, int *channels);
void gfx_free_texture_raw(unsigned char *image);
gfx_texture gfx_load_texture_raw(unsigned char *image, const int width, const int height, const int channels, bool flip, gfx_texture_filtering filter);
gfx_texture gfx_load_texture_resized(const char *filepath, bool flip, gfx_texture_filtering filter, unsigned int w, unsigned int h);
gfx_texture gfx_load_texture_resized_factor(const char *filepath, bool flip, gfx_texture_filtering filter, float wfactor, float hfactor);
gfx_texture gfx_load_texture_from_memory(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter);
gfx_texture gfx_load_texture_from_memory_resized(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter, unsigned int w, unsigned int h);
gfx_texture gfx_load_texture_from_memory_resized_factor(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter, float wfactor, float hfactor);
gfx_texture gfx_load_texture_from_memory_resized_to_fit(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter, int container_w, int container_h);

unsigned char *gfx_load_texture_data(const char *filepath, int *width, int *height, int *channels, bool flip);
unsigned char *gfx_load_texture_data_resized(const char *filepath, int w, int h, int *channels, bool flip);
unsigned char *gfx_load_texture_data_resized_factor(const char *filepath, int wfactor, int hfactor, int *width, int *height, int *channels, bool flip);
unsigned char *gfx_load_texture_data_from_memory(const void *data, unsigned int size, int *width, int *height, int *channels, bool flip);
unsigned char *gfx_load_texture_data_from_memory_resized(const void *data, unsigned int size, int *channels, int *o_w, int *o_h, bool flip, unsigned int w, unsigned int h);
unsigned char *gfx_load_texture_data_from_memory_resized_to_fit_ex(const void *data, unsigned int size, int *o_width, int *o_height, int i_channels, int i_width, int i_height, bool flip, int container_w, int container_h);
unsigned char *gfx_load_texture_data_from_memory_resized_to_fit(const void *data, unsigned int size, int *o_width, int *o_height, int *o_channels, bool flip, int container_w, int container_h);
unsigned char *gfx_load_texture_data_from_memory_resized_factor(const void *data, unsigned int size, int *width, int *height, int *channels, bool flip, float wfactor, float hfactor);

void gfx_create_texture_from_image_data(gfx_texture_filtering filter, unsigned int *id, int width, int height, int channels, unsigned char* data); 
void gfx_free_texture(gfx_texture *tex);
gfx_texture gfx_load_texture_asset(const char *filepath);
gfx_texture gfx_load_texture_asset_raw(unsigned char *image, const int width, const int height, const int channels);

void gfx_free_font(gfx_font *font);
gfx_font gfx_load_font_asset(const char *filepath, const unsigned int font_size);

