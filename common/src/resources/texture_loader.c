#include "khg_gfx/texture.h"
#include "khg_utl/algorithm.h"
#include "threading/resource_loading.h"
#include "resources/texture_loader.h"
#include <stdio.h>
#include <string.h>

static gfx_texture NO_TEXTURE = { 0 };
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

const unsigned int get_tex_id_from_string(const char *tex_key) {
  return utl_algorithm_find_at(TEXTURE_STRINGS, TEXTURE_STRINGS_SIZE, sizeof(char *), tex_key, compare_texture_strings);
}

void add_texture() {
  const texture_asset ta = TEXTURE_ASSET_REF[TEXTURE_LOAD_PROGRESS];
  TEXTURE_LOOKUP[TEXTURE_LOAD_PROGRESS] = generate_texture(ta.tex_filepath, ta.tex_width, ta.tex_height);
  TEXTURE_LOAD_PROGRESS++;
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
  TEXTURE_ASSET_REF[GROUND_GRASS_GREEN] = (texture_asset){ "res/assets/textures/tiles/grounds/grass_green.png", 725, 628 };
  TEXTURE_ASSET_REF[GROUND_GRASS_BLUE] = (texture_asset){ "res/assets/textures/tiles/grounds/grass_blue.png", 725, 628 };
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

void reset_textures() {
  for (unsigned int i = 0; i < NUM_TEXTURES; i++) {
    TEXTURE_LOOKUP[i].id = NO_TEXTURE.id;
  }
}

void load_texture_tick(const unsigned int count) {
  for (unsigned int i = 0; i < count; i++) {
    if (TEXTURE_LOAD_PROGRESS < NUM_TEXTURES) {
      /*printf("LOADED: %i\n", TEXTURE_LOAD_PROGRESS);*/
      add_texture();
    }
  }
}

