#include "gfx/stb.h"
#include "gfx/elements.h"
#include "gfx/renderer.h"
#include "glad/glad.h"
#include "khg_utl/error_func.h"
#include <wctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype/stb_truetype.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize/stb_image_resize2.h>

static void renderer_add_glyph(stbtt_aligned_quad q, int32_t max_descended_char_height, LfColor color, uint32_t tex_index) {
  vec2s texcoords[4] = {
    q.s0, q.t0, 
    q.s1, q.t0, 
    q.s1, q.t1, 
    q.s0, q.t1
  };
  vec2s verts[4] = {
    (vec2s){ q.x0, q.y0 + max_descended_char_height }, 
    (vec2s){ q.x1, q.y0 + max_descended_char_height }, 
    (vec2s){ q.x1, q.y1 + max_descended_char_height },
    (vec2s){ q.x0, q.y1 + max_descended_char_height }
  }; 
  for(uint32_t i = 0; i < 4; i++) {
    if(state.render.vert_count >= MAX_RENDER_BATCH) {
      renderer_flush();
      renderer_begin();
    }
    const vec2 verts_arr = {verts[i].x, verts[i].y};
    memcpy(state.render.verts[state.render.vert_count].pos, verts_arr, sizeof(vec2));
    const vec4 border_color = {0, 0, 0, 0};
    memcpy(state.render.verts[state.render.vert_count].border_color, border_color, sizeof(vec4));
    state.render.verts[state.render.vert_count].border_width = 0;
    vec4s color_zto = lf_color_to_zto(color);
    const vec4 color_arr = {color_zto.r, color_zto.g, color_zto.b, color_zto.a};
    memcpy(state.render.verts[state.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = {texcoords[i].x, texcoords[i].y};
    memcpy(state.render.verts[state.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].tex_index = tex_index;
    const vec2 scale_arr = {0, 0};
    memcpy(state.render.verts[state.render.vert_count].scale, scale_arr, sizeof(vec2));
    const vec2 pos_px_arr = {0, 0};
    memcpy(state.render.verts[state.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].corner_radius = 0;
    const vec2 cull_start_arr = {state.cull_start.x, state.cull_start.y};
    const vec2 cull_end_arr = {state.cull_end.x, state.cull_end.y};
    memcpy(state.render.verts[state.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    memcpy(state.render.verts[state.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    state.render.vert_count++;
  }
  state.render.index_count += 6; 
}

int32_t get_max_char_height_font(LfFont font) {
  float fontScale = stbtt_ScaleForPixelHeight((stbtt_fontinfo *)font.font_info, font.font_size);
  int32_t xmin, ymin, xmax, ymax;
  int32_t codepoint = 'p';
  stbtt_GetCodepointBitmapBox((stbtt_fontinfo *)font.font_info, codepoint, fontScale, fontScale, &xmin, &ymin, &xmax, &ymax);
  return ymax - ymin;
}

LfFont load_font(const char *filepath, uint32_t pixelsize, uint32_t tex_width, uint32_t tex_height, uint32_t line_gap_add) {
  LfFont font = {0};
  FILE *file = fopen(filepath, "rb");
  if (file == NULL) {
    error_func("Failed to open font file", user_defined_data);
  }
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  uint8_t *buffer = (uint8_t *)malloc(fileSize);
  size_t bytesRead = fread(buffer, 1, fileSize, file);
  fclose(file); 
  if (bytesRead != fileSize) {
    error_func("Failed to read font file", user_defined_data);
    free(buffer);
    LfFont emptyFont = {0};
    return emptyFont;
  }
  font.font_info = malloc(sizeof(stbtt_fontinfo));
  stbtt_InitFont((stbtt_fontinfo*)font.font_info, buffer, stbtt_GetFontOffsetForIndex(buffer, 0));
  stbtt_fontinfo *fontinfo = (stbtt_fontinfo *)font.font_info;
  int numglyphs = fontinfo->numGlyphs;
  uint8_t *bitmap = (uint8_t *)malloc(tex_width * tex_height * sizeof(uint32_t));
  uint8_t *bitmap_4bpp = (uint8_t *)malloc(tex_width * tex_height * 4 * sizeof(uint32_t));
  font.cdata = malloc(sizeof(stbtt_bakedchar) * numglyphs);
  font.tex_width = tex_width;
  font.tex_height = tex_height;
  font.line_gap_add = line_gap_add;
  font.font_size = pixelsize;
  font.num_glyphs = numglyphs;
  stbtt_BakeFontBitmap(buffer, 0, pixelsize, bitmap, tex_width, tex_height, 32, numglyphs, (stbtt_bakedchar *)font.cdata);
  uint32_t bitmap_index = 0;
  for(uint32_t i = 0; i < (uint32_t)(tex_width * tex_height * 4); i++) {
    bitmap_4bpp[i] = bitmap[bitmap_index];
    if((i + 1) % 4 == 0) {
      bitmap_index++;
    }
  }
  glGenTextures(1, &font.bitmap.id);
  glBindTexture(GL_TEXTURE_2D, font.bitmap.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap_4bpp);
  glGenerateMipmap(GL_TEXTURE_2D);
  free(buffer);
  free(bitmap);
  free(bitmap_4bpp);
  return font;
}

LfFont get_current_font() {
  return state.font_stack ? *state.font_stack : state.theme.font;
}

LfTexture lf_load_texture(const char *filepath, bool flip, LfTextureFiltering filter) {
  LfTexture tex;
  int width, height, channels;
  unsigned char* image = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);
  if (!image) {
    error_func("Failed to load texture", user_defined_data);
    return tex;
  }
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  switch(filter) {
    case LF_TEX_FILTER_LINEAR:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case LF_TEX_FILTER_NEAREST:
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

LfTexture lf_load_texture_resized(const char *filepath, bool flip, LfTextureFiltering filter, uint32_t w, uint32_t h) {
  LfTexture tex; 
  int32_t width, height, channels;
  stbi_uc *image_data = stbi_load(filepath, &width, &height, &channels, 0);
  unsigned char *downscaled_image = (unsigned char*)malloc(sizeof(unsigned char) * w * h * channels);
  stbir_resize_uint8_linear(image_data, width, height, 0, downscaled_image, w, h, 0, (stbir_pixel_layout)channels);
  glCreateTextures(GL_TEXTURE_2D, 1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  switch(filter) {
    case LF_TEX_FILTER_LINEAR:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    case LF_TEX_FILTER_NEAREST:
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

LfTexture lf_load_texture_resized_factor(const char *filepath, bool flip, LfTextureFiltering filter, float wfactor, float hfactor) {
  LfTexture tex;
  int32_t width, height, channels;
  stbi_uc *data = lf_load_texture_data_resized_factor(filepath, wfactor, hfactor, &width, &height, &channels, flip);
  if(!data) {
    error_func("Failed to load texture file", user_defined_data);
    return tex;
  }
  int32_t w = width * wfactor;
  int32_t h = height * hfactor;
  lf_create_texture_from_image_data(filter, &tex.id, w, h, channels, data);
  free(data);
  tex.width = w;
  tex.height = h;
  return tex;
}

LfTexture lf_load_texture_from_memory(const void *data, size_t size, bool flip, LfTextureFiltering filter) {
  LfTexture tex;
  int width, height, channels;
  unsigned char* image = stbi_load_from_memory(data, size, &width, &height, &channels, STBI_rgb_alpha);
  if (!image) {
    return tex;
  }
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  switch(filter) {
    case LF_TEX_FILTER_LINEAR:
      glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case LF_TEX_FILTER_NEAREST:
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

LfTexture lf_load_texture_from_memory_resized_factor(const void *data, size_t size, bool flip, LfTextureFiltering filter, float wfactor, float hfactor) {
  LfTexture tex;
  int32_t width, height, channels;
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, &channels, 0);
  int w = width * wfactor;
  int h = height * hfactor;
  unsigned char *resized_data = (unsigned char *)malloc(sizeof(unsigned char) * w * h * channels);
  stbir_resize_uint8_linear(image_data, width, height, 0, resized_data, w, h, 0,(stbir_pixel_layout)channels);
  stbi_image_free(image_data);
  lf_create_texture_from_image_data(LF_TEX_FILTER_LINEAR, &tex.id, w, h, channels, resized_data);
  tex.width = w;
  tex.height = h;
  return tex;
}

LfTexture lf_load_texture_from_memory_resized_to_fit(const void *data, size_t size, bool flip, LfTextureFiltering filter, int32_t container_w, int32_t container_h) {
  LfTexture tex;
  int32_t image_width, image_height, channels;
  stbi_uc *image_data = lf_load_texture_data_from_memory((const stbi_uc *)data, size, &image_width, &image_height, &channels, flip);
  int32_t new_width, new_height;
  unsigned char *resized_data =  lf_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, &new_width, &new_height, channels, image_width, image_height, flip, container_w, container_h);
  stbi_image_free(image_data);
  lf_create_texture_from_image_data(LF_TEX_FILTER_LINEAR, &tex.id, new_width, new_height, channels, resized_data);
  tex.width = new_width;
  tex.height = new_height;
  return tex;
}

unsigned char *lf_load_texture_data(const char *filepath, int32_t *width, int32_t *height, int32_t *channels, bool flip) {
  stbi_set_flip_vertically_on_load(!flip);
  stbi_uc *data = stbi_load(filepath, width, height, channels, STBI_rgb_alpha);
  return data;
}

unsigned char *lf_load_texture_data_resized(const char *filepath, int32_t w, int32_t h, int32_t *channels, bool flip) {
  int32_t width, height;
  stbi_uc *data = lf_load_texture_data(filepath, &width, &height, channels, flip);
  unsigned char *downscaled_image = (unsigned char*)malloc(sizeof(unsigned char) * w * h * *channels);
  stbir_resize_uint8_linear(data, width, height, *channels, downscaled_image, w, h, 0, (stbir_pixel_layout)*channels);
  stbi_image_free(data);
  return downscaled_image;
}

unsigned char *lf_load_texture_data_resized_factor(const char *filepath, int32_t wfactor, int32_t hfactor, int32_t *width, int32_t *height, int32_t *channels, bool flip) {
  unsigned char *image = stbi_load(filepath, width, height, channels, STBI_rgb_alpha);
  if (!image) {
    return NULL;
  }
  float w = (wfactor * (*width));
  float h = (hfactor * (*height));
  size_t new_size = w * h * (*channels);
  unsigned char *resized_data = (unsigned char *)malloc(new_size);
  if (resized_data == NULL) {
    return NULL;
  }
  stbir_resize_uint8_linear(image, *width, *height, *channels, resized_data, w, h, 0, (stbir_pixel_layout)*channels);
  free(image);
  return resized_data;
 }

unsigned char *lf_load_texture_data_from_memory(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip) {
  stbi_set_flip_vertically_on_load(!flip);
  unsigned char *image = stbi_load_from_memory(data, size, width, height, channels, 0);
  if (!image) {
    return NULL;
  }
  return image;
}

unsigned char *lf_load_texture_data_from_memory_resized(const void *data, size_t size, int32_t *channels, int32_t *o_w, int32_t *o_h, bool flip, uint32_t w, uint32_t h) {
  int32_t width, height;
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, channels, 0);
  unsigned char *resized_data = lf_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, o_w, o_h, *channels, width, height, flip, 48, 48);
  stbi_image_free(image_data);
  return resized_data;
}

unsigned char *lf_load_texture_data_from_memory_resized_to_fit_ex(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t i_channels, int32_t i_width, int32_t i_height, bool flip, int32_t container_w, int32_t container_h) {
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

unsigned char *lf_load_texture_data_from_memory_resized_to_fit(const void *data, size_t size, int32_t *o_width, int32_t *o_height, int32_t *o_channels, bool flip, int32_t container_w, int32_t container_h) {
  int32_t image_width, image_height, channels;
  stbi_uc *image_data = lf_load_texture_data_from_memory((const stbi_uc *)data, size, &image_width, &image_height, &channels, flip);
  int32_t new_width, new_height;
  unsigned char *resized_data =  lf_load_texture_data_from_memory_resized_to_fit_ex(image_data, size, &new_width, &new_height, channels, image_width, image_height, flip, container_w, container_h);
  *o_width = new_width;
  *o_height = new_height;
  *o_channels = channels;
  stbi_image_free(image_data);
  return image_data;
}

unsigned char *lf_load_texture_data_from_memory_resized_factor(const void *data, size_t size, int32_t *width, int32_t *height, int32_t *channels, bool flip, float wfactor, float hfactor) {
  stbi_uc *image_data = stbi_load_from_memory((const stbi_uc *)data, size, width, height, channels, 0);
  int w = (*width) * wfactor;
  int h = (*height) * hfactor;
  unsigned char *resized_data = (unsigned char *)malloc(sizeof(unsigned char) * w * h * (*channels));
  stbir_resize_uint8_linear(image_data, *width, *height, 0, resized_data, w, h, 0, (stbir_pixel_layout)*channels);
  stbi_image_free(image_data);
  return resized_data;
}

LfTextProps lf_text_render_wchar(vec2s pos, const wchar_t *str, LfFont font, LfColor color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index) {
  bool culled = item_should_cull((LfAABB){ .pos = (vec2s){ pos.x, pos.y + get_current_font().font_size }, .size = (vec2s){ -1, -1 } });
  float tex_index = -1.0f;
  if (!culled && !no_render) {
    if (state.render.tex_count - 1 >= MAX_TEX_COUNT_BATCH - 1) {
      renderer_flush();
      renderer_begin();
    }
    for (uint32_t i = 0; i < state.render.tex_count; i++) {
      if (state.render.textures[i].id == font.bitmap.id) {
        tex_index = (float)i;
        break;
      }
    }
    if (tex_index == -1.0f) {
      tex_index = (float)state.render.tex_index;
      LfTexture tex = font.bitmap;
      state.render.textures[state.render.tex_count++] = tex;
      state.render.tex_index++;
    }
  }
  LfTextProps ret = { 0 };
  float x = pos.x;
  float y = pos.y;
  int32_t max_descended_char_height = get_max_char_height_font(font);
  float last_x = x;
  float height = get_max_char_height_font(font);
  float width = 0;
  uint32_t i = 0;
  while (str[i] != L'\0') {
    if (str[i] >= font.num_glyphs) {
      i++;
      continue;
    }
    if (stbtt_FindGlyphIndex((const stbtt_fontinfo*)font.font_info, str[i] - 32) == 0 && 
      str[i] != L' ' && str[i] != L'\n' && str[i] != L'\t' && !iswdigit(str[i]) && !iswpunct(str[i])) {
      i++;
      continue;
    }
    if (i >= end_index && end_index != -1) {
      break;
    }
    float word_width = 0;
    uint32_t j = i;
    while (str[j] != L' ' && str[j] != L'\n' && str[j] != L'\0') {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad((stbtt_bakedchar *)font.cdata, font.tex_width, font.tex_height, str[j] - 32, &word_width, &y, &q, 0);
      j++;
    }
    if (x + word_width > wrap_point && wrap_point != -1) {
      y += font.font_size;
      height += font.font_size;
      if (x - pos.x > width) {
        width = x - pos.x;
      }
      x = pos.x;
      last_x = x;
    }
    if (str[i] == L'\n') {
      y += font.font_size;
      height += font.font_size;
      if (x - pos.x > width) {
        width = x - pos.x;
      }
      x = pos.x;
      last_x = x;
      i++;
      continue;
    }
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad((stbtt_bakedchar *)font.cdata, font.tex_width, font.tex_height, str[i] - 32, &x, &y, &q, 1);
    if (i < start_index && start_index != -1) {
      last_x = x;
      ret.rendered_count++;
      i++;
      continue;
    }
    if (stop_point.x != -1 && stop_point.y != -1) {
      if (x >= stop_point.x && stop_point.x != -1 && y + get_max_char_height_font(font) >= stop_point.y && stop_point.y != -1) {
        break;
      }
    } 
    else {
      if (y + get_max_char_height_font(font) >= stop_point.y && stop_point.y != -1) {
        break;
      }
    }
    if (!culled && !no_render && state.renderer_render) {
      if (render_solid) {
        lf_rect_render((vec2s){ x, y }, (vec2s){ last_x - x, get_max_char_height_font(font) }, color, LF_NO_COLOR, 0.0f, 0.0f);
      } 
      else {
        renderer_add_glyph(q, max_descended_char_height, color, tex_index);
      }
      last_x = x;
    }
    ret.rendered_count++;
    i++;
  }
  if (x - pos.x > width) {
    width = x - pos.x;
  }
  ret.width = width;
  ret.height = height;
  ret.end_x = x;
  ret.end_y = y;
  return ret;
}
