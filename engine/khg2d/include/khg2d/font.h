#pragma once

#include "khgmath/vec4.h"
#include "stb_truetype/stb_truetype.h"
#include "texture.h"

typedef struct {
  texture texture;
  vec2 size;
  stbtt_packedchar *packed_chars_buffer;
  int packed_chars_buffer_size;
  float max_height;
} font;

stbtt_aligned_quad font_get_glyph_quad(const font font, const char c);
vec4 font_get_glyph_texture_doords(const font font, const char c);
void create_from_TTF(font *f, const unsigned char *ttf_data, const size_t ttf_data_size);
void create_from_file(font *f, const char *file);
void cleanup_font(font *f);
