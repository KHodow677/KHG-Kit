#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else 
#endif

#include "glad/glad.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/font.h"
#include "khg_utl/error_func.h"
#include "stb_image/stb_image.h"
#include "stb_image_resize/stb_image_resize2.h"
#include <string.h>
#include <unistd.h>

gfx_font gfx_load_font(const char *filepath, unsigned int size) {
  return gfx_internal_load_font(filepath, size, 1024, 1024, 0);
}

gfx_font gfx_load_font_ex(const char *filepath, unsigned int size, unsigned int bitmap_w, unsigned int bitmap_h) {
  return gfx_internal_load_font(filepath, size, bitmap_w, bitmap_h, 0);
}

gfx_texture gfx_load_texture(const char *filepath, bool flip, gfx_texture_filtering filter) {
  gfx_texture tex;
  int width, height, channels;
  unsigned char *image = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);
  if (!image) {
    utl_error_func("Failed to load a texture", utl_user_defined_data);
    return tex;
  }
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  switch(filter) {
    case GFX_TEX_FILTER_LINEAR:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case GFX_TEX_FILTER_NEAREST:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image);
  tex.width = width;
  tex.height = height;
  return tex;
}


void gfx_fetch_texture_raw(unsigned char **image, const char *filepath, int *width, int *height, int *channels) {
  *image = stbi_load(filepath, width, height, channels, STBI_rgb_alpha);
}

void gfx_free_texture_raw(unsigned char *image) {
  stbi_image_free(image);
}

gfx_texture gfx_load_texture_raw(unsigned char *image, const int width, const int height, const int channels, bool flip, gfx_texture_filtering filter) {
  gfx_texture tex;
  if (!image) {
    utl_error_func("Failed to load a texture", utl_user_defined_data);
    return tex;
  }
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  switch(filter) {
    case GFX_TEX_FILTER_LINEAR:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case GFX_TEX_FILTER_NEAREST:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  tex.width = width;
  tex.height = height;
  return tex;
}

gfx_texture gfx_load_texture_resized(const char *filepath, bool flip, gfx_texture_filtering filter, unsigned int w, unsigned int h) {
  gfx_texture tex; 
  int width, height, channels;
  stbi_uc *image_data = stbi_load(filepath, &width, &height, &channels, 0);
  unsigned char *downscaled_image = (unsigned char*)malloc(sizeof(unsigned char) * w * h * channels);
  stbir_resize_uint8_linear(image_data, width, height, 0, downscaled_image, w, h, 0,(stbir_pixel_layout)channels);
  glCreateTextures(GL_TEXTURE_2D, 1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  switch(filter) {
    case GFX_TEX_FILTER_LINEAR:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    case GFX_TEX_FILTER_NEAREST:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, downscaled_image);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image_data);
  free(downscaled_image);
  tex.width = width;
  tex.height = height;
  return tex;
}

gfx_texture gfx_load_texture_resized_factor(const char *filepath, bool flip, gfx_texture_filtering filter, float wfactor, float hfactor) {
  gfx_texture tex;
  int width, height, channels;
  stbi_uc *data = gfx_load_texture_data_resized_factor(filepath, wfactor, hfactor, &width, &height, &channels, flip);
  if(!data) {
    utl_error_func("Failed to load texture file", utl_user_defined_data);
    return tex;
  }
  int w = width * wfactor;
  int h = height * hfactor;
  gfx_create_texture_from_image_data(filter, &tex.id, w, h, channels, data);
  free(data);
  tex.width = w;
  tex.height = h;
  return tex;
}

gfx_texture gfx_load_texture_from_memory(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter) {
  gfx_texture tex;
  int width, height, channels;
  unsigned char *image = stbi_load_from_memory(data, size, &width, &height, &channels, STBI_rgb_alpha);
  if (!image) {
    return tex;
  }
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  switch(filter) {
    case GFX_TEX_FILTER_LINEAR:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case GFX_TEX_FILTER_NEAREST:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image);
  tex.width = width;
  tex.height = height;
  return tex;
}

gfx_texture gfx_load_texture_from_memory_resized(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter, unsigned int w, unsigned int h) {
  gfx_texture tex;
  int channels;
  unsigned char *resized = gfx_load_texture_data_from_memory_resized(data, size, &channels, NULL, NULL, flip, w, h);
  gfx_create_texture_from_image_data(GFX_TEX_FILTER_LINEAR, &tex.id, w, h, channels, resized);
  tex.width = w;
  tex.height = h;
  return tex;
}

gfx_texture gfx_load_texture_from_memory_resized_factor(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter, float wfactor, float hfactor) {
  gfx_texture tex;
  int width, height, channels;
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, &channels, 0);
  int w = width * wfactor;
  int h = height * hfactor;
  unsigned char *resized_data = (unsigned char*)malloc(sizeof(unsigned char) * w * h * channels);
  stbir_resize_uint8_linear(image_data, width, height, 0, resized_data, w, h, 0,(stbir_pixel_layout)channels);
  stbi_image_free(image_data);
  gfx_create_texture_from_image_data(GFX_TEX_FILTER_LINEAR, &tex.id, w, h, channels, resized_data);
  tex.width = w;
  tex.height = h;
  return tex;
}

gfx_texture gfx_load_texture_from_memory_resized_to_fit(const void *data, unsigned int size, bool flip, gfx_texture_filtering filter, int container_w, int container_h) {
  gfx_texture tex;
  int image_width, image_height, channels;
  stbi_uc *image_data = gfx_load_texture_data_from_memory((const stbi_uc*)data, size, &image_width, &image_height, &channels, flip);
  int new_width, new_height;
  unsigned char *resized_data =  gfx_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, &new_width, &new_height, channels, image_width, image_height, flip, container_w, container_h);
  stbi_image_free(image_data);
  gfx_create_texture_from_image_data(GFX_TEX_FILTER_LINEAR, &tex.id, new_width, new_height, channels, resized_data);
  tex.width = new_width;
  tex.height = new_height;
  return tex;
}

unsigned char *gfx_load_texture_data(const char *filepath, int *width, int *height, int *channels, bool flip) {
  stbi_set_flip_vertically_on_load(!flip);
  stbi_uc *data = stbi_load(filepath, width, height, channels, STBI_rgb_alpha);
  return data;
}

unsigned char *gfx_load_texture_data_resized(const char *filepath, int w, int h, int *channels, bool flip) {
  int width, height;
  stbi_uc *data = gfx_load_texture_data(filepath, &width, &height, channels, flip);
  unsigned char *downscaled_image = (unsigned char *)malloc(sizeof(unsigned char) * w * h * *channels);
  stbir_resize_uint8_linear(data, width, height, *channels, downscaled_image, w, h, 0, (stbir_pixel_layout)*channels);
  stbi_image_free(data);
  return downscaled_image;
}

unsigned char *gfx_load_texture_data_resized_factor(const char *filepath, int wfactor, int hfactor, int *width, int *height, int *channels, bool flip) {
  unsigned char *image = stbi_load(filepath, width, height, channels, STBI_rgb_alpha);
  if (!image) {
    return NULL;
  }
  float w = (wfactor * (*width));
  float h = (hfactor * (*height));
  unsigned int new_size = w * h * (*channels);
  unsigned char *resized_data = (unsigned char*)malloc(new_size);
  if (resized_data == NULL) {
    return NULL;
  }
  stbir_resize_uint8_linear(image, *width, *height, *channels, resized_data, w, h, 0, (stbir_pixel_layout)*channels);
  free(image);
  return resized_data;
}

unsigned char *gfx_load_texture_data_from_memory(const void *data, unsigned int size, int *width, int *height, int *channels, bool flip) {
  stbi_set_flip_vertically_on_load(!flip);
  unsigned char *image = stbi_load_from_memory(data, size, width, height, channels, 0);
  if (!image) {
    return NULL;
  }
  return image;
}

unsigned char *gfx_load_texture_data_from_memory_resized(const void *data, unsigned int size, int *channels, int *o_w, int *o_h, bool flip, unsigned int w, unsigned int h) {
  int width, height;
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, channels, 0);
  unsigned char *resized_data = gfx_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, o_w, o_h, *channels, width, height, flip, 48, 48);
  stbi_image_free(image_data);
  return resized_data;
}

unsigned char *gfx_load_texture_data_from_memory_resized_to_fit_ex(const void *data, unsigned int size, int *o_width, int *o_height, int i_channels, int i_width, int i_height, bool flip, int container_w, int container_h) {
  float container_aspect_ratio = (float)container_w / container_h;
  float image_aspect_ratio = (float)i_width / i_height;
  int new_width, new_height;
  if (image_aspect_ratio > container_aspect_ratio) {
    new_width = container_w;
    new_height = (int)((container_w / (float)i_width) * i_height);
  } 
  else {
    new_height = container_h;
    new_width = (int)((container_h / (float)i_height) * i_width);
  }
  if(o_width) {
    *o_width = new_width;
  }
  if(o_height) {
    *o_height = new_height;
  }
  unsigned char *resized_image = (unsigned char *)malloc(sizeof(unsigned char) * new_width * new_height * i_channels);
  stbir_resize_uint8_linear(data, i_width, i_height, 0, resized_image, new_width, new_height, 0, (stbir_pixel_layout)i_channels);
  return resized_image;
}

unsigned char *gfx_load_texture_data_from_memory_resized_to_fit(const void *data, unsigned int size, int *o_width, int *o_height, int *o_channels, bool flip, int container_w, int container_h) {
  int image_width, image_height, channels;
  stbi_uc *image_data = gfx_load_texture_data_from_memory((const stbi_uc *)data, size, &image_width, &image_height, &channels, flip);
  int new_width, new_height;
  unsigned char *resized_data =  gfx_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, &new_width, &new_height, channels, image_width, image_height, flip, container_w, container_h);
  *o_width = new_width;
  *o_height = new_height;
  *o_channels = channels;
  stbi_image_free(image_data);
  return image_data;
}

unsigned char *gfx_load_texture_data_from_memory_resized_factor(const void *data, unsigned int size, int *width, int *height, int *channels, bool flip, float wfactor, float hfactor) {
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, width, height, channels, 0);
  int w = (*width) * wfactor;
  int h = (*height) * hfactor;
  unsigned char *resized_data = (unsigned char *)malloc(sizeof(unsigned char) * w * h * (*channels));
  stbir_resize_uint8_linear(image_data, *width, *height, 0, resized_data, w, h, 0, (stbir_pixel_layout)*channels);
  stbi_image_free(image_data);
  return resized_data;
}

void gfx_create_texture_from_image_data(gfx_texture_filtering filter, unsigned int *id, int width, int height, int channels, unsigned char *data) {
  GLenum internal_format = (channels == 4) ? GL_RGBA8 : GL_RGB8;
  GLenum data_format = (channels == 4) ? GL_RGBA : GL_RGB;
  glGenTextures(1, id);
  glBindTexture(GL_TEXTURE_2D, *id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  switch(filter) {
    case GFX_TEX_FILTER_LINEAR:
      glTextureParameteri(*id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(*id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case GFX_TEX_FILTER_NEAREST:
      glTextureParameteri(*id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(*id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, data_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void gfx_free_texture(gfx_texture *tex) {
  glDeleteTextures(1, &tex->id);
  memset(tex, 0, sizeof(gfx_texture));
}

gfx_texture gfx_load_texture_asset(const char *filepath) {
  return gfx_load_texture(filepath, false, GFX_TEX_FILTER_LINEAR);
}

gfx_texture gfx_load_texture_asset_raw(unsigned char *image, const int width, const int height, const int channels) {
  return gfx_load_texture_raw(image, width, height, channels, false, GFX_TEX_FILTER_LINEAR);
}

void gfx_free_font(gfx_font *font) {
  free(font->cdata);
  free(font->font_info);
  free(font->buffer);
}

gfx_font gfx_load_font_asset(const char *filepath, const unsigned int font_size) {
  return gfx_load_font(filepath, font_size);
}

