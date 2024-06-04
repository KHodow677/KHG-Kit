#include "khg2d.h"
#include "khg2d.c"
#include <string.h>

const unsigned char getOld(const unsigned char *decodedImage, int width, int x, int y, int c) {
  return decodedImage[4 * (x + (y * width)) + c];
}

const unsigned char getNew(const unsigned char *newData, int newW, int x, int y, int c) {
  return newData[4 * (x + (y * newW)) + c];
}

void changeOld(unsigned char *decodedImage, int width, int x1, int y, int c1, int x2, int y2, int c2) {
  decodedImage[4 * (x1 + (y * width)) + c1] = getOld(decodedImage, width, x2, y, c2);
}

void changeNew(unsigned char *newData, int newW, int x1, int y, int c1, int x2, int y2, int c2) {
  newData[4 * (x1 + (y * newW)) + c1] = getNew(newData, newW, x2, y, c2);
}

vec2 getSize(texture *t) {
  	vec2 s;
		glBindTexture(GL_TEXTURE_2D, t->id);
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

void create1PxSquare(texture *t, const char *b) {
  if (b == NULL) {
    const unsigned char buff[] = { 0xff, 0xff, 0xff, 0xff };
    createFromBuffer(t, (char *)buff, 1, 1, false, false);
  }
  else {
    createFromBuffer(t, b, 1, 1, false, false);
  }
}

void createFromFileData(texture *t, const unsigned char *image_file_data, const size_t image_file_size, bool pixelated, bool useMipMaps) {
  int width = 0;
  int height = 0;
  int channels = 0; 
  const unsigned char * decodedImage;
  stbi_set_flip_vertically_on_load(true);
  decodedImage = stbi_load_from_memory(image_file_data, (int)image_file_size, &width, &height, &channels, 4);
  createFromBuffer(t, (const char*)decodedImage, width, height, pixelated, useMipMaps);
  stbi_image_free((void *)decodedImage);
}

void createFromFileDataWithPixelPadding(texture *t, const unsigned char *image_file_data, const size_t image_file_size, int blockSize, bool pixelated, bool useMipMaps) {
 	int width = 0, height = 0, channels = 0, x, y;
  const unsigned char* decodedImage;
  int newW, newH;
  unsigned char *newData;
  int newDataCursor = 0, dataCursor = 0;
  stbi_set_flip_vertically_on_load(true);
  decodedImage = stbi_load_from_memory(image_file_data, (int)image_file_size, &width, &height, &channels, 4);
  newW = width + ((width * 2) / blockSize);
  newH = height + ((height * 2) / blockSize);
  newData = malloc(newW * newH * 4);
  newDataCursor = 0;
  dataCursor = 0;
  for (y = 0; y < newH; y++) {
    int yNo = 0;
    if ((y == 0 || y == newH - 1 || ((y) % (blockSize + 2)) == 0 || ((y + 1) % (blockSize + 2)) == 0)) {
      yNo = 1;
    }
    for (x = 0; x < newW; x++) {
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
  for (x = 1; x < newW - 1; x++) {
    if (x == 1 || (x % (blockSize + 2)) == 1) {
      for (y = 0; y < newH; y++) { 
        changeNew(newData, newW, x - 1, y, 0, x, y, 0);
        changeNew(newData, newW, x - 1, y, 1, x, y, 1);
        changeNew(newData, newW, x - 1, y, 2, x, y, 2);
        changeNew(newData, newW, x - 1, y, 3, x, y, 3);
      }
    } 
    else if (x == newW - 2 || (x % (blockSize + 2)) == blockSize) {
      for (y = 0; y < newH; y++) {  
        changeNew(newData, newW, x + 1, y, 0, x, y, 0);
        changeNew(newData, newW, x + 1, y, 1, x, y, 1);
        changeNew(newData, newW, x + 1, y, 2, x, y, 2);
        changeNew(newData, newW, x + 1, y, 3, x, y, 3);
      }
    }
  }
  for (y = 1; y < newH - 1; y++) {
    if (y == 1 || (y % (blockSize + 2)) == 1) {
      for (x = 0; x < newW; x++) {
        changeNew(newData, newW, x, y - 1, 0, x, y, 0);
        changeNew(newData, newW, x, y - 1, 1, x, y, 1);
        changeNew(newData, newW, x, y - 1, 2, x, y, 2);
        changeNew(newData, newW, x, y - 1, 3, x, y, 3);
      }
    } 
    else if (y == newH - 2 || (y % (blockSize + 2)) == blockSize) {
      for (x = 0; x < newW; x++) {
        changeNew(newData, newW, x, y + 1, 0, x, y, 0);
        changeNew(newData, newW, x, y + 1, 1, x, y, 1);
        changeNew(newData, newW, x, y + 1, 2, x, y, 2);
        changeNew(newData, newW, x, y + 1, 3, x, y, 3);
      }
    }
  }
  createFromBuffer(t, (const char*)newData, newW, newH, pixelated, useMipMaps);
  stbi_image_free((void *)decodedImage);
  free(newData);
}

void loadFromFile(texture *t, const char *fileName, bool pixelated, bool useMipMaps) {
  FILE *file = fopen(fileName, "rb");
  long fileSize;
  unsigned char *fileData;
  if (!file) {
    char c[300] = { 0 };
		strcat(c, "error openning: ");
		strcat(c + strlen(c), fileName);
		errorFunc(c, (void *)userDefinedData);
		return;
  }
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  fileData = (unsigned char *)malloc(fileSize);
  if (fileData == NULL) {
    fclose(file);
    errorFunc("memory allocation failed", (void *)setUserDefinedData);
    return;
  }
  fread(fileData, 1, fileSize, file);
  fclose(file);
  createFromFileData(t, fileData, fileSize, pixelated, useMipMaps);
  free(fileData);
}

void loadFromFileWithPixelPadding(texture *t, const char *fileName, int blockSize, bool pixelated, bool useMipMaps) {
  FILE *file = fopen(fileName, "rb");
  long fileSize;
  unsigned char * fileData;
  if (!file) {
    char c[300] = { 0 };
		strcat(c, "error openning: ");
		strcat(c + strlen(c), fileName);
		errorFunc(c, (void *)userDefinedData);
		return;
  }
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  fileData = (unsigned char *)malloc(fileSize);
  if (fileData == NULL) {
    fclose(file);
    errorFunc("memory allocation failed", (void *)setUserDefinedData);
    return;
  }
  fread(fileData, 1, fileSize, file);
  fclose(file);
  createFromFileDataWithPixelPadding(t, fileData, fileSize, blockSize, pixelated, useMipMaps);
  free(fileData);
}

size_t getMemorySize(texture *t, int mipLevel, vec2 *outSize) {
  vec2 stub = { 0 };
  glBindTexture(GL_TEXTURE_2D, t->id);
  if (!outSize) {
    outSize = &stub;
  }
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &outSize->x);
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &outSize->y);
  glBindTexture(GL_TEXTURE_2D, 0);
  return outSize->x * outSize->y * 4;
}

void readTextureData(texture *t, void *buffer, int mipLevel) {
  glBindTexture(GL_TEXTURE_2D, t->id);
  glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
}

unsigned char *readTextureDataToCharArray(texture *t, int mipLevel, vec2 *outSize) {
  vec2 stub = { 0 };
  unsigned char *data;
  glBindTexture(GL_TEXTURE_2D, t->id);
  if (!outSize) {
    outSize = &stub;
  }
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &outSize->x);
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &outSize->y);
  data = (unsigned char*)malloc(outSize->x * outSize->y * 4 * sizeof(unsigned char));
  if (data == NULL) {
    glBindTexture(GL_TEXTURE_2D, 0);
    return NULL;
  }
  glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
  return data;
}

void bindTexture(texture *t, const unsigned int sample) {
  glActiveTexture(GL_TEXTURE0 + sample);
  glBindTexture(GL_TEXTURE_2D, t->id);
}

void unbindTexture(texture *t) {
  glBindTexture(GL_TEXTURE_2D, 0);
}

void cleanupTexture(texture *t) {
  glDeleteTextures(1, &t->id);
  memset(t, 0, sizeof(texture));
}


