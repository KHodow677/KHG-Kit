#include "khg_ui/internal.h"
#include "khg_ui/texture.h"

ui_font ui_load_font(const char *filepath, uint32_t size) {
  return load_font(filepath, size, 1024, 1024, 0);
}

ui_font ui_load_font_ex(const char *filepath, uint32_t size, uint32_t bitmap_w, uint32_t bitmap_h) {
  return load_font(filepath, size, bitmap_w, bitmap_h, 0);
}

ui_texture ui_load_texture(const char *filepath, bool flip, ui_texture_filtering filter) {
  ui_texture tex;
  int width, height, channels;
  unsigned char *image = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);
  if (!image) {
    error_func("Failed to load texture", user_defined_data);
    return tex;
  }
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  switch(filter) {
    case ui_tex_filter_linear:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case ui_tex_filter_nearest:
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

ui_texture ui_load_texture_resized(const char *filepath, bool flip, ui_texture_filtering filter, uint32_t w, uint32_t h) {
  ui_texture tex; 
  int32_t width, height, channels;
  stbi_uc *image_data = stbi_load(filepath, &width, &height, &channels, 0);
  unsigned char *downscaled_image = (unsigned char*)malloc(sizeof(unsigned char) * w * h * channels);
  stbir_resize_uint8_linear(image_data, width, height, 0, downscaled_image, w, h, 0,(stbir_pixel_layout)channels);
  glCreateTextures(GL_TEXTURE_2D, 1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  switch(filter) {
    case ui_tex_filter_linear:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    case ui_tex_filter_nearest:
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

ui_texture ui_load_texture_resized_factor(const char *filepath, bool flip, ui_texture_filtering filter, float wfactor, float hfactor) {
  ui_texture tex;
  int32_t width, height, channels;
  stbi_uc *data = ui_load_texture_data_resized_factor(filepath, wfactor, hfactor, &width, &height, &channels, flip);
  if(!data) {
    error_func("Failed to load texture file", user_defined_data);
    return tex;
  }
  int32_t w = width * wfactor;
  int32_t h = height * hfactor;
  ui_create_texture_from_image_data(filter, &tex.id, w, h, channels, data);
  free(data);
  tex.width = w;
  tex.height = h;
  return tex;
}

ui_texture ui_load_texture_from_memory(const void *data, size_t size, bool flip, ui_texture_filtering filter) {
  ui_texture tex;
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
    case ui_tex_filter_linear:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case ui_tex_filter_nearest:
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

ui_texture ui_load_texture_from_memory_resized(const void *data, size_t size, bool flip, ui_texture_filtering filter, uint32_t w, uint32_t h) {
  ui_texture tex;
  int32_t channels;
  unsigned char *resized = ui_load_texture_data_from_memory_resized(data, size, &channels, NULL, NULL, flip, w, h);
  ui_create_texture_from_image_data(ui_tex_filter_linear, &tex.id, w, h, channels, resized);
  tex.width = w;
  tex.height = h;
  return tex;
}

ui_texture ui_load_texture_from_memory_resized_factor(const void *data, size_t size, bool flip, ui_texture_filtering filter, float wfactor, float hfactor) {
  ui_texture tex;
  int32_t width, height, channels;
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, &channels, 0);
  int w = width * wfactor;
  int h = height * hfactor;
  unsigned char *resized_data = (unsigned char*)malloc(sizeof(unsigned char) * w * h * channels);
  stbir_resize_uint8_linear(image_data, width, height, 0, resized_data, w, h, 0,(stbir_pixel_layout)channels);
  stbi_image_free(image_data);
  ui_create_texture_from_image_data(ui_tex_filter_linear, &tex.id, w, h, channels, resized_data);
  tex.width = w;
  tex.height = h;
  return tex;
}

ui_texture ui_load_texture_from_memory_resized_to_fit(const void *data, size_t size, bool flip, ui_texture_filtering filter, int32_t container_w, int32_t container_h) {
  ui_texture tex;
  int32_t image_width, image_height, channels;
  stbi_uc *image_data = ui_load_texture_data_from_memory((const stbi_uc*)data, size, &image_width, &image_height, &channels, flip);
  int32_t new_width, new_height;
  unsigned char *resized_data =  ui_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, &new_width, &new_height, channels, image_width, image_height, flip, container_w, container_h);
  stbi_image_free(image_data);
  ui_create_texture_from_image_data(ui_tex_filter_linear, &tex.id, new_width, new_height, channels, resized_data);
  tex.width = new_width;
  tex.height = new_height;
  return tex;
}

unsigned char *ui_load_texture_data(const char *filepath, int32_t *width, int32_t *height, int32_t *channels, bool flip) {
  stbi_set_flip_vertically_on_load(!flip);
  stbi_uc *data = stbi_load(filepath, width, height, channels, STBI_rgb_alpha);
  return data;
}

unsigned char *ui_load_texture_data_resized(const char *filepath, int32_t w, int32_t h, int32_t *channels, bool flip) {
  int32_t width, height;
  stbi_uc *data = ui_load_texture_data(filepath, &width, &height, channels, flip);
  unsigned char *downscaled_image = (unsigned char *)malloc(sizeof(unsigned char) * w * h * *channels);
  stbir_resize_uint8_linear(data, width, height, *channels, downscaled_image, w, h, 0, (stbir_pixel_layout)*channels);
  stbi_image_free(data);
  return downscaled_image;
}

unsigned char *ui_load_texture_data_resized_factor(const char *filepath, int32_t wfactor, int32_t hfactor, int32_t *width, int32_t *height, int32_t *channels, bool flip) {
  unsigned char *image = stbi_load(filepath, width, height, channels, STBI_rgb_alpha);
  if (!image) {
    return NULL;
  }
  float w = (wfactor * (*width));
  float h = (hfactor * (*height));
  size_t new_size = w * h * (*channels);
  unsigned char *resized_data = (unsigned char*)malloc(new_size);
  if (resized_data == NULL) {
    return NULL;
  }
  stbir_resize_uint8_linear(image, *width, *height, *channels, resized_data, w, h, 0, (stbir_pixel_layout)*channels);
  free(image);
  return resized_data;
}

unsigned char *ui_load_texture_data_from_memory(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip) {
  stbi_set_flip_vertically_on_load(!flip);
  unsigned char *image = stbi_load_from_memory(data, size, width, height, channels, 0);
  if (!image) {
    return NULL;
  }
  return image;
}

unsigned char *ui_load_texture_data_from_memory_resized(const void *data, size_t size, int32_t *channels, int32_t *o_w, int32_t *o_h, bool flip, uint32_t w, uint32_t h) {
  int32_t width, height;
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, channels, 0);
  unsigned char *resized_data = ui_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, o_w, o_h, *channels, width, height, flip, 48, 48);
  stbi_image_free(image_data);
  return resized_data;
}

unsigned char *ui_load_texture_data_from_memory_resized_to_fit_ex(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t i_channels, int32_t i_width, int32_t i_height, bool flip, int32_t container_w, int32_t container_h) {
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

unsigned char *ui_load_texture_data_from_memory_resized_to_fit(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t *o_channels, bool flip, int32_t container_w, int32_t container_h) {
  int32_t image_width, image_height, channels;
  stbi_uc *image_data = ui_load_texture_data_from_memory((const stbi_uc *)data, size, &image_width, &image_height, &channels, flip);
  int32_t new_width, new_height;
  unsigned char *resized_data =  ui_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, &new_width, &new_height, channels, image_width, image_height, flip, container_w, container_h);
  *o_width = new_width;
  *o_height = new_height;
  *o_channels = channels;
  stbi_image_free(image_data);
  return image_data;
}

unsigned char *ui_load_texture_data_from_memory_resized_factor(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip, float wfactor, float hfactor) {
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, width, height, channels, 0);
  int w = (*width) * wfactor;
  int h = (*height) * hfactor;
  unsigned char *resized_data = (unsigned char *)malloc(sizeof(unsigned char) * w * h * (*channels));
  stbir_resize_uint8_linear(image_data, *width, *height, 0, resized_data, w, h, 0, (stbir_pixel_layout)*channels);
  stbi_image_free(image_data);
  return resized_data;
}

void ui_create_texture_from_image_data(ui_texture_filtering filter, uint32_t *id, int32_t width, int32_t height, int32_t channels, unsigned char *data) {
  GLenum internal_format = (channels == 4) ? GL_RGBA8 : GL_RGB8;
  GLenum data_format = (channels == 4) ? GL_RGBA : GL_RGB;
  glGenTextures(1, id);
  glBindTexture(GL_TEXTURE_2D, *id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  switch(filter) {
    case ui_tex_filter_linear:
      glTextureParameteri(*id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(*id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case ui_tex_filter_nearest:
      glTextureParameteri(*id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(*id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, data_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void ui_free_texture(ui_texture *tex) {
  glDeleteTextures(1, &tex->id);
  memset(tex, 0, sizeof(ui_texture));
}

ui_texture ui_load_texture_asset(const char *asset_name, const char *file_extension) {
  char asset_dir[strlen(getenv(HOMEDIR)) + strlen("/.asset")];
  memset(asset_dir, 0, sizeof(asset_dir));
  strcat(asset_dir, getenv(HOMEDIR));
  strcat(asset_dir, "/.asset");
  char path[strlen(asset_dir) + strlen("/assets/textures/") + strlen(asset_name) + strlen(".") + strlen(file_extension)];
  memset(path, 0, sizeof(path));
  strcat(path, asset_dir);
  strcat(path, "/assets/textures/");
  strcat(path, asset_name);
  strcat(path, ".");
  strcat(path, file_extension);
  return ui_load_texture(path, false, ui_tex_filter_linear);
}

void ui_free_font(ui_font *font) {
  free(font->cdata);
  free(font->font_info);
}

ui_font ui_load_font_asset(const char *asset_name, const char *file_extension, uint32_t font_size) {
  char asset_dir[strlen(getenv(HOMEDIR)) + strlen("/.asset")];
  memset(asset_dir, 0, sizeof(asset_dir));
  strcat(asset_dir, getenv(HOMEDIR));
  strcat(asset_dir, "/.asset");
  char path[strlen(asset_dir) + strlen("/assets/fonts/") + strlen(asset_name) + strlen(".") + strlen(file_extension)];
  memset(path, 0, sizeof(path));
  strcat(path, asset_dir);
  strcat(path, "/assets/fonts/");
  strcat(path, asset_name);
  strcat(path, ".");
  strcat(path, file_extension);
  return ui_load_font(path, font_size);
}
