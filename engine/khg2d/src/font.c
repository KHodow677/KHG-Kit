#define STB_TRUETYPE_IMPLEMENTATION

#include "khg2d/font.h"
#include "khg2d/utils.h"
#include <string.h>

stbtt_aligned_quad fontGetGlyphQuad(const font f, const char c) {
  stbtt_aligned_quad quad;
  float x = 0, y = 0;
  stbtt_GetPackedQuad(f.packedCharsBuffer, f.size.x, f.size.y, c - ' ', &x, &y, &quad, 1);
  return quad;
}

vec4 fontGetGlyphTextureCoords(const font f, const char c) {
  const stbtt_aligned_quad quad = fontGetGlyphQuad(f, c);
  vec4 quadVec = { quad.s0, quad.t0, quad.s1, quad.t1 };
  return quadVec;
}

void createFromTTF(font *f, const unsigned char *ttf_data, const size_t ttf_data_size) {
  size_t fontMonochromeBufferSize, fontRgbaBufferSize;
  unsigned char *fontMonochromeBuffer, *fontRgbaBuffer;
  stbtt_pack_context stbtt_context;
  int i;
  char c;
  f->size = (vec2){ 2000.0f, 2000.0f };
  f->maxHeight = 0,
  f->packedCharsBufferSize = ('~' - ' ');
  fontMonochromeBufferSize = f->size.x * f->size.y;
  fontRgbaBufferSize = f->size.x * f->size.y * 4;
  fontMonochromeBuffer = (unsigned char *)malloc(fontMonochromeBufferSize);
  fontRgbaBuffer = (unsigned char *)malloc(fontRgbaBufferSize);
  f->packedCharsBuffer = malloc(f->packedCharsBufferSize);
  stbtt_PackBegin(&stbtt_context, fontMonochromeBuffer, f->size.x, f->size.y, 0, 2, NULL);
  stbtt_PackSetOversampling(&stbtt_context, 2, 2);
  stbtt_PackFontRange(&stbtt_context, ttf_data, 0, 65, ' ', '~' - ' ', f->packedCharsBuffer);
  stbtt_PackEnd(&stbtt_context);
  for (i = 0; i < fontMonochromeBufferSize; i++) {
    fontRgbaBuffer[(i * 4)] = fontMonochromeBuffer[i];
    fontRgbaBuffer[(i * 4) + 1] = fontMonochromeBuffer[i];
    fontRgbaBuffer[(i * 4) + 2] = fontMonochromeBuffer[i];
    if (fontMonochromeBuffer[i] > 1) {
      fontRgbaBuffer[(i * 4) + 3] = 255;
    }
    else {
      fontRgbaBuffer[(i * 4) + 3] = 0;
    }
  }
  glGenTextures(1, &f->texture.id);
  glBindTexture(GL_TEXTURE_2D, f->texture.id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, f->size.x, f->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontRgbaBuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  free(fontMonochromeBuffer);
  free(fontRgbaBuffer);
  for (c = ' '; c <= '~'; c++) {
    const stbtt_aligned_quad q =  fontGetGlyphQuad(*f, c);
    const float m = q.y1 - q.y0;
    if (m > f->maxHeight && m < 1.e+8f) {
      f->maxHeight = m;
    }
  }
}

void createFromFile(font *f, const char *file) {
  FILE *fileFont = fopen(file, "rb");
  if (!fileFont) {
    char c[300] = {0};
    strcat(c, "error opening: ");
    strcat(c + strlen(c), file);
    errorFunc(c, userDefinedData);
    return;
  }
  fseek(fileFont, 0, SEEK_END);
  long fileSize = ftell(fileFont);
  fseek(fileFont, 0, SEEK_SET);
  unsigned char *fileData = (unsigned char *)malloc(fileSize);
  if (fileData == NULL) {
    fclose(fileFont);
    return;
  }
  fread(fileData, 1, fileSize, fileFont);
  fclose(fileFont);
  createFromTTF(f, fileData, fileSize);
  free(fileData);
}

void cleanupFont(font *f) {
  cleanupTexture(&f->texture);
  memset(f, 0, sizeof(font));
}
