#include "khg2d.h"

void createFromTTF(font *f, const unsigned char *ttf_data, const size_t ttf_data_size) {
  f->size.x = 2000,
  f->size.y = 2000,
  f->maxHeight = 0,
  f->packedCharsBufferSize = ('~' - ' ');
  const size_t fontMonochromeBufferSize = f->size.x * f->size.y;
  const size_t fontRgbaBufferSize = f->size.x * f->size.y * 4;
  unsigned char *fontMonochromeBuffer = malloc(fontMonochromeBufferSize);
  unsigned char *fontRgbaBuffer = malloc(fontRgbaBufferSize);
  f->packedCharsBuffer = malloc(f->packedCharsBufferSize);
  stbtt_pack_context stbtt_context;
  stbtt_PackBegin(&stbtt_context, fontMonochromeBuffer, f->size.x, f->size.y, 0, 2, NULL);
  stbtt_PackSetOversampling(&stbtt_context, 2, 2);
  stbtt_PackFontRange(&stbtt_context, ttf_data, 0, 65, ' ', '~' - ' ', f->packedCharsBuffer);
  stbtt_PackEnd(&stbtt_context);
  for (int i = 0; i < fontMonochromeBufferSize; i++) {
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
  {
    glGenTextures(1, &f->texture.id);
    glBindTexture(GL_TEXTURE_2D, f->texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, f->size.x, f->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontRgbaBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  free(fontMonochromeBuffer);
  free(fontRgbaBuffer);
  for (char c = ' '; c <= '~'; c++) {
    const stbtt_aligned_quad q = fontGetGlyphQuad(*f, c);
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
    errorFunc("memory allocation failed", userDefinedData);
    return;
  }
  fread(fileData, 1, fileSize, fileFont);
  fclose(fileFont);
  createFromTTF(f, fileData, fileSize);
  free(fileData);
}

void cleanupFont(font *f) {
  cleanupTexture(f->texture);
  memset(f, 0, sizeof(font));
}
