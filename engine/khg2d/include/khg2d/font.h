#pragma once

#include "khgmath/vec4.h"
#include "stb_truetype/stb_truetype.h"
#include "texture.h"

typedef struct {
  texture texture;
  vec2 size;
  stbtt_packedchar *packedCharsBuffer;
  int packedCharsBufferSize;
  float maxHeight;
} font;

stbtt_aligned_quad fontGetGlyphQuad(const font font, const char c);
vec4 fontGetGlyphTextureCoords(const font font, const char c);
void createFromTTF(font *f, const unsigned char *ttf_data, const size_t ttf_data_size);
void createFromFile(font *f, const char *file);
void cleanupFont(font *f);
