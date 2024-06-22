#define STB_TRUETYPE_IMPLEMENTATION

#include "khgutils/error_func.h"
#include "khg2d/font.h"
#include <string.h>

stbtt_aligned_quad font_get_glyph_quad(const font f, const char c) {
  stbtt_aligned_quad quad;
  float x = 0, y = 0;
  stbtt_GetPackedQuad(f.packed_chars_buffer, f.size.x, f.size.y, c - ' ', &x, &y, &quad, 1);
  return quad;
}

vec4 font_get_glyph_texture_doords(const font f, const char c) {
  const stbtt_aligned_quad quad = font_get_glyph_quad(f, c);
  vec4 quad_vec = { quad.s0, quad.t0, quad.s1, quad.t1 };
  return quad_vec;
}

void create_from_TTF(font *f, const unsigned char *ttf_data, const size_t ttf_data_size) {
  size_t font_monochrome_buffer_size, font_rgba_buffer_size;
  unsigned char *font_monochrome_buffer, *font_rgba_buffer;
  stbtt_pack_context stbtt_context;
  int i;
  char c;
  f->size = (vec2){ 2000.0f, 2000.0f };
  f->max_height = 0,
  f->packed_chars_buffer_size = ('~' - ' ');
  font_monochrome_buffer_size = f->size.x * f->size.y;
  font_rgba_buffer_size = f->size.x * f->size.y * 4;
  font_monochrome_buffer = (unsigned char *)malloc(font_monochrome_buffer_size);
  font_rgba_buffer = (unsigned char *)malloc(font_rgba_buffer_size);
  f->packed_chars_buffer = malloc(f->packed_chars_buffer_size);
  stbtt_PackBegin(&stbtt_context, font_monochrome_buffer, f->size.x, f->size.y, 0, 2, NULL);
  stbtt_PackSetOversampling(&stbtt_context, 2, 2);
  stbtt_PackFontRange(&stbtt_context, ttf_data, 0, 65, ' ', '~' - ' ', f->packed_chars_buffer);
  stbtt_PackEnd(&stbtt_context);
  for (i = 0; i < font_monochrome_buffer_size; i++) {
    font_rgba_buffer[(i * 4)] = font_monochrome_buffer[i];
    font_rgba_buffer[(i * 4) + 1] = font_monochrome_buffer[i];
    font_rgba_buffer[(i * 4) + 2] = font_monochrome_buffer[i];
    if (font_monochrome_buffer[i] > 1) {
      font_rgba_buffer[(i * 4) + 3] = 255;
    }
    else {
      font_rgba_buffer[(i * 4) + 3] = 0;
    }
  }
  glGenTextures(1, &f->texture.id);
  glBindTexture(GL_TEXTURE_2D, f->texture.id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, f->size.x, f->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, font_rgba_buffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  free(font_monochrome_buffer);
  free(font_rgba_buffer);
  for (c = ' '; c <= '~'; c++) {
    const stbtt_aligned_quad q =  font_get_glyph_quad(*f, c);
    const float m = q.y1 - q.y0;
    if (m > f->max_height && m < 1.e+8f) {
      f->max_height = m;
    }
  }
}

void create_from_file(font *f, const char *file) {
  FILE *file_font = fopen(file, "rb");
  if (!file_font) {
    char c[300] = {0};
    strcat(c, "error opening: ");
    strcat(c + strlen(c), file);
    error_func(c, user_defined_data);
    return;
  }
  fseek(file_font, 0, SEEK_END);
  long file_size = ftell(file_font);
  fseek(file_font, 0, SEEK_SET);
  unsigned char *file_data = (unsigned char *)malloc(file_size);
  if (file_data == NULL) {
    fclose(file_font);
    return;
  }
  fread(file_data, 1, file_size, file_font);
  fclose(file_font);
  create_from_TTF(f, file_data, file_size);
  free(file_data);
}

void cleanup_font(font *f) {
  cleanup_texture(&f->texture);
  memset(f, 0, sizeof(font));
}
