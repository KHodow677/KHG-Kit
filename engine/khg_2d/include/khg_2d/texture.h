#pragma once

#include "khg_math/vec2.h"
#include "khg_utils/vector.h"
#include "glad/glad.h"
#include "stb_image/stb_image.h"
#include <stdbool.h>

typedef struct {
  GLuint id;
} texture;

vec2 get_texture_size(texture *t);
void create_from_buffer(texture *t, const char *image_data, const int width, const int height, bool pixelated, bool use_mip_maps);
void create_1_px_square(texture *t, const char *b);
void create_from_file_data(texture *t, const unsigned char *image_file_data, const size_t image_file_size, bool pixelated, bool use_mip_maps);
void create_from_file_data_with_pixel_padding(texture *t, const unsigned char *image_file_data, const size_t image_file_size, int block_size, bool pixelated, bool use_mip_maps);
void load_from_file(texture *t, const char *file_name, bool pixelated, bool use_mip_maps);
void load_from_file_with_pixel_padding(texture *t, const char *file_name, int block_size, bool pixelated, bool use_mip_maps);
size_t get_memory_size(texture *t, int mip_level, vec2 *out_size);
void read_texture_data(texture *t, void *buffer, int mip_level);
vector read_texture_data_to_char_array(texture *t, int mip_level, vec2 *out_size);
void bind_texture(texture *t, const unsigned int sample);
void unbind_texture(texture *t);
void cleanup_texture(texture *t);
