#include "khg_gfx/texture.h"
#include "khg_utl/algorithm.h"
#include "threading/resource_loading.h"
#include "resources/texture_loader.h"
#include <stdio.h>
#include <string.h>

static texture_raw_info TEXTURE_RAW_LOOKUP[NUM_TEXTURES];
static gfx_texture TEXTURE_LOOKUP[NUM_TEXTURES];
static texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

static int compare_texture_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const gfx_texture generate_texture(const char *filepath, const float width, const float height) {
  gfx_texture tex = gfx_load_texture_asset(filepath);
  tex.width = width;
  tex.height = height;
  tex.angle = 0;
  return tex;
}

const gfx_texture generate_texture_raw(texture_raw_info raw_info) {
  gfx_texture tex = gfx_load_texture_asset_raw(raw_info.tex_raw, raw_info.width, raw_info.height, raw_info.channels);
  tex.width = raw_info.width;
  tex.height = raw_info.height;
  tex.angle = 0;
  return tex;
}

const unsigned int get_tex_id_from_string(const char *tex_key) {
  return utl_algorithm_find_at(TEXTURE_STRINGS, TEXTURE_STRINGS_SIZE, sizeof(char *), tex_key, compare_texture_strings);
}

void add_texture_raw() {
  const texture_asset ta = TEXTURE_ASSET_REF[TEXTURE_RAW_THREAD.progress];
  int width, height, channels;
  unsigned char *tex_raw;
  gfx_fetch_texture_raw(&tex_raw, ta.tex_filepath, &width, &height, &channels);
  TEXTURE_RAW_LOOKUP[TEXTURE_RAW_THREAD.progress] = (texture_raw_info){ tex_raw, ta.tex_width, ta.tex_height, channels };
  TEXTURE_RAW_THREAD.progress++;
}

void add_texture() {
  const texture_asset ta = TEXTURE_ASSET_REF[TEXTURE_THREAD.progress];
  TEXTURE_LOOKUP[TEXTURE_THREAD.progress] = generate_texture_raw(TEXTURE_RAW_LOOKUP[TEXTURE_THREAD.progress]);
  gfx_free_texture_raw(TEXTURE_RAW_LOOKUP[TEXTURE_THREAD.progress].tex_raw);
  TEXTURE_THREAD.progress++;
}

const gfx_texture get_texture(const unsigned int tex_id) {
  return TEXTURE_LOOKUP[tex_id];
}

const gfx_texture get_texture_from_string(const char *tex_key) {
  const unsigned int tex_id = get_tex_id_from_string(tex_key);
  return get_texture(tex_id);
}

void generate_textures() {
  TEXTURE_ASSET_REF[EMPTY_TEXTURE] = (texture_asset){ "res/assets/textures/square.png", 512, 512 };
  TEXTURE_ASSET_REF[GROUND_GRASS] = (texture_asset){ "res/assets/textures/tiles/grounds/grass.png", 725, 628 };
  TEXTURE_ASSET_REF[BORDER_BLACK] = (texture_asset){ "res/assets/textures/tiles/borders/black.png", 795, 688 };
  for (unsigned int i = ELEMENT_BUSH_0, count = 0; i <= ELEMENT_BUSH_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/bushes/bush_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_DIRTPATCH_0, count = 0; i <= ELEMENT_DIRTPATCH_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/dirt_patches/dirt_patch_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_GRASSBUSH_DARK_0, count = 0; i <= ELEMENT_GRASSBUSH_DARK_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/grass_bushes/dark/grass_bush_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_GRASSBUSH_REG_0, count = 0; i <= ELEMENT_GRASSBUSH_REG_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/grass_bushes/reg/grass_bush_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_GRASSMARKS_0, count = 0; i <= ELEMENT_GRASSMARKS_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/grass_marks/grass_marks_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_GRASSPATCH_0, count = 0; i <= ELEMENT_GRASSPATCH_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/grass_patches/grass_patch_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_TALLGRASS_0, count = 0; i <= ELEMENT_TALLGRASS_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/tall_grass/tall_grass_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
}

int load_texture_raw_tick(void *arg) {
  resource_thread *thread = arg;
  if (thread->progress < NUM_TEXTURES) {
    add_texture_raw();
  }
  return 0;
}

int load_texture_tick(void *arg) {
  resource_thread *thread = arg;
  for (unsigned int i = 0; i < thread->max; i++) {
    if (thread->progress < NUM_TEXTURES) {
      add_texture();
    }
  }
  return 0;
}

