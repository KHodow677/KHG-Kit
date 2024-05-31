#include "khg2d.h"

const unsigned char getOld(const unsigned char *decodedImage, int width, int x, int y, int c) {
  return decodedImage[4 * (x + (y * width)) + c];
}

const unsigned char getNew(const unsigned char *newData, int newW, int x, int y, int c) {
  return newData[4 * (x + (y * newW)) + c];
}

void changeOld(unsigned char *decodedImage, int width, int x1, int y1, int c1, int x2, int y2, int c2) {
  decodedImage[4 * (x1 + (y1 * width)) + c1] = getOld(decodedImage, width, x2, y2, c2);
}

void changeNew(unsigned char *newData, int newW, int x1, int y1, int c1, int x2, int y2, int c2) {
  newData[4 * (x1 + (y1 * newW)) + c1] = getNew(newData, newW, x2, y2, c2);
}

vec2 getSize(texture *texture) {
  	vec2 s;
		glBindTexture(GL_TEXTURE_2D, texture->id);
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &s.x);
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &s.y);
		return s;
}

void createFromBuffer(texture *t, const char *image_data, const int width, const int height, bool pixelated, bool useMipMaps) {
  	GLuint id = 0;
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &t->id);
		glBindTexture(GL_TEXTURE_2D, t->id);
		if (pixelated) {
			if (useMipMaps) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else {
			if (useMipMaps) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		t->id = id;
}

void create1PxSquare(texture *texture, const char *b) {
  if (b == NULL) {
    const unsigned char buff[] = { 0xff, 0xff, 0xff, 0xff };
    createFromBuffer(texture, (char *)buff, 1, 1, false, false);
  }
  else {
    createFromBuffer(texture, b, 1, 1, false, false);
  }
}

void createFromFileData(texture *t, const unsigned char *image_file_data, const size_t image_file_size, bool pixelated, bool useMipMaps) {
  stbi_set_flip_vertically_on_load(true);
  int width = 0;
  int height = 0;
  int channels = 0;
  const unsigned char* decodedImage = stbi_load_from_memory(image_file_data, (int)image_file_size, &width, &height, &channels, 4);
  createFromBuffer(t, (const char*)decodedImage, width, height, pixelated, useMipMaps);
  stbi_image_free((void *)decodedImage);
}

void createFromFileDataWithPixelPadding(texture *t, const unsigned char *image_file_data, const size_t image_file_size, int blockSize, bool pixelated, bool useMipMaps) {
 		stbi_set_flip_vertically_on_load(true);
		int width = 0;
		int height = 0;
		int channels = 0;
		const unsigned char* decodedImage = stbi_load_from_memory(image_file_data, (int)image_file_size, &width, &height, &channels, 4);
		int newW = width + ((width * 2) / blockSize);
		int newH = height + ((height * 2) / blockSize);
		unsigned char* newData = malloc(newW * newH * 4);
		int newDataCursor = 0;
		int dataCursor = 0;
    for (int y1 = 0; y1 < newH; y1++) {
      int yNo = 0;
      if ((y1 == 0 || y1 == newH - 1 || ((y1) % (blockSize + 2)) == 0 || ((y1 + 1) % (blockSize + 2)) == 0)) {
        yNo = 1;
      }
      for (int x = 0; x < newW; x++) {
        if (yNo || ((x == 0 || x == newW - 1 || (x % (blockSize + 2)) == 0 || ((x + 1) % (blockSize + 2)) == 0))) {
          newData[newDataCursor++] = 0;
          newData[newDataCursor++] = 0;
          newData[newDataCursor++] = 0;
          newData[newDataCursor++] = 0;
        } 
        else {
          newData[newDataCursor++] = decodedImage[dataCursor++];
          newData[newDataCursor++] = decodedImage[dataCursor++];
          newData[newDataCursor++] = decodedImage[dataCursor++];
          newData[newDataCursor++] = decodedImage[dataCursor++];
        }
      }
    }
    for (int x = 1; x < newW - 1; x++) {
      if (x == 1 || (x % (blockSize + 2)) == 1) {
        for (int y2 = 0; y2 < newH; y2++) { 
          changeNew(newData, newW, x - 1, y2, 0, x, y2, 0);
          changeNew(newData, newW, x - 1, y2, 1, x, y2, 1);
          changeNew(newData, newW, x - 1, y2, 2, x, y2, 2);
          changeNew(newData, newW, x - 1, y2, 3, x, y2, 3);
        }
      } 
      else if (x == newW - 2 || (x % (blockSize + 2)) == blockSize) {
        for (int y2 = 0; y2 < newH; y2++) {  
          changeNew(newData, newW, x + 1, y2, 0, x, y2, 0);
          changeNew(newData, newW, x + 1, y2, 1, x, y2, 1);
          changeNew(newData, newW, x + 1, y2, 2, x, y2, 2);
          changeNew(newData, newW, x + 1, y2, 3, x, y2, 3);
        }
      }
    }

    for (int y3 = 1; y3 < newH - 1; y3++) {
      if (y3 == 1 || (y3 % (blockSize + 2)) == 1) {
        for (int x = 0; x < newW; x++) {
          changeNew(newData, newW, x, y3 - 1, 0, x, y3, 0);
          changeNew(newData, newW, x, y3 - 1, 1, x, y3, 1);
          changeNew(newData, newW, x, y3 - 1, 2, x, y3, 2);
          changeNew(newData, newW, x, y3 - 1, 3, x, y3, 3);
        }
      } 
      else if (y3 == newH - 2 || (y3 % (blockSize + 2)) == blockSize) {
        for (int x = 0; x < newW; x++) {
          changeNew(newData, newW, x, y3 + 1, 0, x, y3, 0);
          changeNew(newData, newW, x, y3 + 1, 1, x, y3, 1);
          changeNew(newData, newW, x, y3 + 1, 2, x, y3, 2);
          changeNew(newData, newW, x, y3 + 1, 3, x, y3, 3);
        }
      }
    }
		createFromBuffer(t, (const char*)newData, newW, newH, pixelated, useMipMaps);
		stbi_image_free((void *)decodedImage);
		free(newData);
}

void loadFromFile(texture *t, const char *fileName, bool pixelated, bool useMipMaps);
void loadFromFileWithPixelPadding(texture *t, const char *fileName, int blockSize, bool pixelated, bool useMipMaps);
size_t getMemorySize(texture *t, int mipLevel, vec2 *outSize);
void readTextureData(texture *t, void *buffer, int mipLevel);
unsigned char *readTextureDataToCharArray(texture *t, int mipLevel, vec2 *outSize);
void bindTexture(texture *texture, const unsigned int sample);
void unbindTexture(texture *texture);
void cleanupTexture(texture *texture);


