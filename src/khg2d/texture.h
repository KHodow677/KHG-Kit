#pragma once

#include "glad/glad.h"
#include "stb/stb_image.h"
#include "../utils/bool.h"
#include "../math/vec2.h"

typedef struct {
  GLuint id;
} texture;

vec2 getSize(texture *t);
void createFromBuffer(texture *t, const char *image_data, const int width, const int height, bool pixelated, bool useMipMaps);
void create1PxSquare(texture *t, const char *b);
void createFromFileData(texture *t, const unsigned char *image_file_data, const size_t image_file_size, bool pixelated, bool useMipMaps);
void createFromFileDataWithPixelPadding(texture *t, const unsigned char *image_file_data, const size_t image_file_size, int blockSize, bool pixelated, bool useMipMaps);
void loadFromFile(texture *t, const char *fileName, bool pixelated, bool useMipMaps);
void loadFromFileWithPixelPadding(texture *t, const char *fileName, int blockSize, bool pixelated, bool useMipMaps);
size_t getMemorySize(texture *t, int mipLevel, vec2 *outSize);
void readtextureData(texture *t, void *buffer, int mipLevel);
unsigned char *readtextureDataToCharArray(texture *t, int mipLevel, vec2 *outSize);
void bindTexture(texture *t, const unsigned int sample);
void unbindTexture(texture *t);
void cleanupTexture(texture *t);
