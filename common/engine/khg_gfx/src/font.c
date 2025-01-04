#define STB_TRUETYPE_IMPLEMENTATION

#include "glad/glad.h"
#include "khg_gfx/font.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/ui.h"
#include "khg_utl/error_func.h"
#include "stb_truetype/stb_truetype.h"
#include <stdio.h>
#include <wctype.h>

static void renderer_add_glyph(stbtt_aligned_quad q, int max_descended_char_height, gfx_color color, unsigned int tex_index) {
  vec2s texcoords[4] = { q.s0, q.t0, q.s1, q.t0, q.s1, q.t1, q.s0, q.t1 };
  vec2s verts[4] = { 
    (vec2s){ q.x0, q.y0 + max_descended_char_height }, 
    (vec2s){ q.x1, q.y0 + max_descended_char_height }, 
    (vec2s){ q.x1, q.y1 + max_descended_char_height },
    (vec2s){ q.x0, q.y1 + max_descended_char_height }
  }; 
  for (unsigned int i = 0; i < 4; i++) {
    if(GFX_STATE.render.vert_count >= GFX_MAX_RENDER_BATCH) {
      gfx_internal_renderer_flush();
      gfx_internal_renderer_begin();
    }
    const vec2 verts_arr = { verts[i].x, verts[i].y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].pos, verts_arr, sizeof(vec2));
    const vec4 border_color = { 0, 0, 0, 0 };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].border_color, border_color, sizeof(vec4));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].border_width = 0;
    vec4s color_zto = gfx_color_to_zto(color);
    const vec4 color_arr = { color_zto.r, color_zto.g, color_zto.b, color_zto.a };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = { texcoords[i].x, texcoords[i].y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].tex_index = tex_index;
    const vec2 scale_arr = { 0, 0 };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].scale, scale_arr, sizeof(vec2));
    const vec2 pos_px_arr = { 0, 0 };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].corner_radius = 0;
    const vec2 cull_start_arr = { GFX_STATE.cull_start.x, GFX_STATE.cull_start.y };
    const vec2 cull_end_arr = { GFX_STATE.cull_end.x, GFX_STATE.cull_end.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    GFX_STATE.render.vert_count++;
  }
  GFX_STATE.render.index_count += 6; 
}

gfx_text_props gfx_text_render_char(vec2s pos, const char *str, gfx_font font, gfx_color color, int wrap_point, vec2s stop_point, bool no_render, bool render_solid, int start_index, int end_index) {
  bool culled = gfx_internal_item_should_cull((gfx_aabb){ .pos = (vec2s){ pos.x, pos.y + gfx_internal_get_current_font().font_size }, .size = (vec2s){ -1, -1 } }, true);
  float tex_index = -1.0f;
  if (!culled && !no_render) {
    if (GFX_STATE.render.tex_count - 1 >= GFX_MAX_TEX_COUNT_BATCH - 1) {
      gfx_internal_renderer_flush();
      gfx_internal_renderer_begin();
    }
    for (unsigned int i = 0; i < GFX_STATE.render.tex_count; i++) {
      if (GFX_STATE.render.textures[i].id == font.bitmap.id) {
        tex_index = (float)i;
        break;
      }
    }
    if (tex_index == -1.0f) {
      tex_index = (float)GFX_STATE.render.tex_index;
      gfx_texture tex = font.bitmap;
      GFX_STATE.render.textures[GFX_STATE.render.tex_count++] = tex;
      GFX_STATE.render.tex_index++;
    }
  }
  gfx_text_props ret = { 0 };
  float x = pos.x;
  float y = pos.y;
  int max_descended_char_height = get_max_char_height_font(font);
  float last_x = x;
  float height = get_max_char_height_font(font);
  float width = 0;
  int i = 0;
  while (str[i] != L'\0') {
    if (str[i] >= font.num_glyphs) {
      i++;
      continue;
    }
    if (stbtt_FindGlyphIndex((const stbtt_fontinfo *)font.font_info, str[i] - 32) == 0 && 
      str[i] != L' ' && str[i] != L'\n' && str[i] != L'\t' && !iswdigit(str[i]) && !iswpunct(str[i])) {
      i++;
      continue;
    }
    if (i >= end_index && end_index != -1) {
      break;
    }
    float word_width = 0;
    unsigned int j = i;
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
    if (!culled && !no_render && GFX_STATE.renderer_render) {
      if (render_solid) {
        gfx_rect_render((vec2s){ x, y }, (vec2s){ last_x - x, get_max_char_height_font(font) }, color, GFX_NO_COLOR, 0.0f, 0.0f, 0.0f);
      } else {
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

gfx_font gfx_internal_load_font(const char *filepath, unsigned int pixelsize, unsigned int tex_width, unsigned int tex_height, unsigned int line_gap_add) {
  gfx_font font = { 0 };
  FILE *file = fopen(filepath, "rb");
  if (file == NULL) {
    utl_error_func("Failed to open font file", utl_user_defined_data);
  }
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  unsigned char *buffer = (unsigned char *)malloc(fileSize);
  unsigned int bytesRead = fread(buffer, 1, fileSize, file);
  fclose(file); 
  if (bytesRead != fileSize) {
    utl_error_func("Failed to read font file", utl_user_defined_data);
    free(buffer);
    gfx_font emptyFont = { 0 };
    return emptyFont;
  }
  font.font_info = malloc(sizeof(stbtt_fontinfo));
  stbtt_InitFont((stbtt_fontinfo*)font.font_info, buffer, stbtt_GetFontOffsetForIndex(buffer, 0));
  stbtt_fontinfo *fontinfo = (stbtt_fontinfo *)font.font_info;
  int numglyphs = fontinfo->numGlyphs;
  unsigned char *bitmap = malloc(tex_width * tex_height * sizeof(unsigned int));
  unsigned char *bitmap_4bpp = malloc(tex_width * tex_height * 4 * sizeof(unsigned int));
  font.cdata = malloc(sizeof(stbtt_bakedchar) * numglyphs);
  font.tex_width = tex_width;
  font.tex_height = tex_height;
  font.line_gap_add = line_gap_add;
  font.font_size = pixelsize;
  font.num_glyphs = numglyphs;
  stbtt_BakeFontBitmap(buffer, 0, pixelsize, bitmap, tex_width, tex_height, 32, numglyphs, (stbtt_bakedchar *)font.cdata);
  unsigned int bitmap_index = 0;
  for (unsigned int i = 0; i < (unsigned int)(tex_width * tex_height * 4); i++) {
    bitmap_4bpp[i] = bitmap[bitmap_index];
    if ((i + 1) % 4 == 0) {
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
  font.buffer = buffer;
  free(bitmap);
  free(bitmap_4bpp);
  return font;
}

int get_max_char_height_font(gfx_font font) {
  float fontScale = stbtt_ScaleForPixelHeight((stbtt_fontinfo *)font.font_info, font.font_size);
  int xmin, ymin, xmax, ymax;
  int codepoint = 'p';
  stbtt_GetCodepointBitmapBox((stbtt_fontinfo *)font.font_info, codepoint, fontScale, fontScale, &xmin, &ymin, &xmax, &ymax);
  return ymax - ymin;
}

