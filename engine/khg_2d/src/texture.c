#define STB_IMAGE_IMPLEMENTATION

#include "khg_2d/texture.h"
#include "khg_2d/utils.h"
#include "khg_utils/error_func.h"
#include <string.h>

const unsigned char get_old(const unsigned char *decodedImage, int width, int x, int y, int c) {
  return decodedImage[4 * (x + (y * width)) + c];
}

const unsigned char get_new(const unsigned char *newData, int newW, int x, int y, int c) {
  return newData[4 * (x + (y * newW)) + c];
}

void change_old(unsigned char *decodedImage, int width, int x1, int y, int c1, int x2, int y2, int c2) {
  decodedImage[4 * (x1 + (y * width)) + c1] = get_old(decodedImage, width, x2, y, c2);
}

void change_new(unsigned char *newData, int newW, int x1, int y, int c1, int x2, int y2, int c2) {
  newData[4 * (x1 + (y * newW)) + c1] = get_new(newData, newW, x2, y, c2);
}

vec2 get_texture_size(texture *t) {
  	vec2 s;
		glBindTexture(GL_TEXTURE_2D, t->id);
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &s.x);
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &s.y);
		return s;
}

void create_from_buffer(texture *t, const char *image_data, const int width, const int height, bool pixelated, bool use_mip_maps) {
  	GLuint id = 0;
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		if (pixelated) {
			if (use_mip_maps) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else {
			if (use_mip_maps) {
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

void create_1_px_square(texture *t, const char *b) {
  if (b == NULL) {
    const unsigned char buff[] = { 0xff, 0xff, 0xff, 0xff };
    create_from_buffer(t, (char *)buff, 1, 1, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  }
  else {
    create_from_buffer(t, b, 1, 1, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  }
}

void create_from_file_data(texture *t, const unsigned char *image_file_data, const size_t image_file_size, bool pixelated, bool use_mip_maps) {
  stbi_set_flip_vertically_on_load(true);
  int width = 0, height = 0, channels = 0;
  const unsigned char *decodedImage = stbi_load_from_memory(image_file_data, (int)image_file_size, &width, &height, &channels, 4);
  create_from_buffer(t, (const char*)decodedImage, width, height, pixelated, use_mip_maps);
  stbi_image_free((void *)decodedImage);
}

void create_from_file_data_with_pixel_padding(texture *t, const unsigned char *image_file_data, const size_t image_file_size, int block_size, bool pixelated, bool use_mip_maps) {
  stbi_set_flip_vertically_on_load(true);
 	int width = 0, height = 0, channels = 0;
  const unsigned char *decoded_image = stbi_load_from_memory(image_file_data, (int)image_file_size, &width, &height, &channels, 4);
  int new_w = width + ((width * 2) / block_size);
  int new_h = height + ((height * 2) / block_size);
  unsigned char *new_data = malloc(new_w * new_h * 4);
  int new_data_cursor = 0;
  int data_cursor = 0;
  for (int y = 0; y < new_h; y++) {
    int y_no = 0;
    if ((y == 0 || y == new_h - 1 || ((y) % (block_size + 2)) == 0 || ((y + 1) % (block_size + 2)) == 0)) {
      y_no = 1;
    }
    for (int x = 0; x < new_w; x++) {
      if (y_no || ((x == 0 || x == new_w - 1 || (x % (block_size + 2)) == 0 || ((x + 1) % (block_size + 2)) == 0))) {
        new_data[new_data_cursor++] = 0;
        new_data[new_data_cursor++] = 0;
        new_data[new_data_cursor++] = 0;
        new_data[new_data_cursor++] = 0;
      } 
      else {
        new_data[new_data_cursor++] = decoded_image[data_cursor++];
        new_data[new_data_cursor++] = decoded_image[data_cursor++];
        new_data[new_data_cursor++] = decoded_image[data_cursor++];
        new_data[new_data_cursor++] = decoded_image[data_cursor++];
      }
    }
  }
  for (int x = 1; x < new_w - 1; x++) {
    if (x == 1 || (x % (block_size + 2)) == 1) {
      for (int y = 0; y < new_h; y++) { 
        change_new(new_data, new_w, x - 1, y, 0, x, y, 0);
        change_new(new_data, new_w, x - 1, y, 1, x, y, 1);
        change_new(new_data, new_w, x - 1, y, 2, x, y, 2);
        change_new(new_data, new_w, x - 1, y, 3, x, y, 3);
      }
    } 
    else if (x == new_w - 2 || (x % (block_size + 2)) == block_size) {
      for (int y = 0; y < new_h; y++) {  
        change_new(new_data, new_w, x + 1, y, 0, x, y, 0);
        change_new(new_data, new_w, x + 1, y, 1, x, y, 1);
        change_new(new_data, new_w, x + 1, y, 2, x, y, 2);
        change_new(new_data, new_w, x + 1, y, 3, x, y, 3);
      }
    }
  }
  for (int y = 1; y < new_h - 1; y++) {
    if (y == 1 || (y % (block_size + 2)) == 1) {
      for (int x = 0; x < new_w; x++) {
        change_new(new_data, new_w, x, y - 1, 0, x, y, 0);
        change_new(new_data, new_w, x, y - 1, 1, x, y, 1);
        change_new(new_data, new_w, x, y - 1, 2, x, y, 2);
        change_new(new_data, new_w, x, y - 1, 3, x, y, 3);
      }
    } 
    else if (y == new_h - 2 || (y % (block_size + 2)) == block_size) {
      for (int x = 0; x < new_w; x++) {
        change_new(new_data, new_w, x, y + 1, 0, x, y, 0);
        change_new(new_data, new_w, x, y + 1, 1, x, y, 1);
        change_new(new_data, new_w, x, y + 1, 2, x, y, 2);
        change_new(new_data, new_w, x, y + 1, 3, x, y, 3);
      }
    }
  }
  create_from_buffer(t, (const char*)new_data, new_w, new_h, pixelated, use_mip_maps);
  stbi_image_free((void *)decoded_image);
  free(new_data);
}

void load_from_file(texture *t, const char *file_name, bool pixelated, bool use_mip_maps) {
  FILE *file = fopen(file_name, "rb");
  if (file == NULL) {
    char c[300] = {0};
    snprintf(c, sizeof(c), "error opening: %s", file_name);
    error_func(c, user_defined_data);
    return;
  }
  fseek(file, 0, SEEK_END);
  int file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
    unsigned char *file_data = (unsigned char *)malloc(file_size);
    if (file_data == NULL) {
        fclose(file);
        return;
    }
    fread(file_data, 1, file_size, file);
    fclose(file);
    create_from_file_data(t, file_data, file_size, pixelated, use_mip_maps);
    free(file_data);
}

void load_from_file_with_pixel_padding(texture *t, const char *file_name, int block_size, bool pixelated, bool use_mip_maps) {
  FILE *file = fopen(file_name, "rb");
  if (!file) {
    char c[300] = { 0 };
		strcat(c, "error openning: ");
		strcat(c + strlen(c), file_name);
    error_func(c, user_defined_data);
		return;
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  unsigned char *file_data = (unsigned char *)malloc(file_size);
  if (file_data == NULL) {
    fclose(file);
    return;
  }
  fread(file_data, 1, file_size, file);
  fclose(file);
  create_from_file_data_with_pixel_padding(t, file_data, file_size, block_size, pixelated, use_mip_maps);
  free(file_data);
}

size_t get_memory_size(texture *t, int mip_level, vec2 *out_size) {
  vec2 stub = { 0 };
  glBindTexture(GL_TEXTURE_2D, t->id);
  if (!out_size) {
    out_size = &stub;
  }
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mip_level, GL_TEXTURE_WIDTH, &out_size->x);
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mip_level, GL_TEXTURE_HEIGHT, &out_size->y);
  glBindTexture(GL_TEXTURE_2D, 0);
  return out_size->x * out_size->y * 4;
}

void read_texture_data(texture *t, void *buffer, int mip_level) {
  glBindTexture(GL_TEXTURE_2D, t->id);
  glGetTexImage(GL_TEXTURE_2D, mip_level, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
}

vector(unsigned char) read_texture_data_to_char_array(texture *t, int mip_level, vec2 *out_size) {
  vec2 stub = { 0 };
  vector(unsigned char) data = NULL;
  glBindTexture(GL_TEXTURE_2D, t->id);
  if (!out_size) {
    out_size = &stub;
  }
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mip_level, GL_TEXTURE_WIDTH, &out_size->x);
  glGetTexLevelParameterfv(GL_TEXTURE_2D, mip_level, GL_TEXTURE_HEIGHT, &out_size->y);
  vector_resize(data, out_size->x * out_size->y * 4, ' ');
  glGetTexImage(GL_TEXTURE_2D, mip_level, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
  return data;
}

void bind_texture(texture *t, const unsigned int sample) {
  glActiveTexture(GL_TEXTURE0 + sample);
  glBindTexture(GL_TEXTURE_2D, t->id);
}

void unbind_texture(texture *t) {
  glBindTexture(GL_TEXTURE_2D, 0);
}

void cleanup_texture(texture *t) {
  glDeleteTextures(1, &t->id);
  memset(t, 0, sizeof(texture));
}

