#include "khg_gfx/texture.h"
#include "khg_utl/algorithm.h"
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

const bool check_texture_loaded(const unsigned int tex_id) {
  return (tex_id == NULL_TEXTURE || TEXTURE_LOOKUP[tex_id].id != NO_TEXTURE.id);
}

const unsigned int get_tex_id_from_string(const char *tex_key) {
  return utl_algorithm_find_at(TEXTURE_STRINGS, TEXTURE_STRINGS_SIZE, sizeof(char *), tex_key, compare_texture_strings);
}

const gfx_texture get_or_add_texture(const unsigned int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return TEXTURE_LOOKUP[tex_id];
  }
  const texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  TEXTURE_LOOKUP[tex_id] = generate_texture(ta.tex_filepath, ta.tex_width, ta.tex_height);
  return TEXTURE_LOOKUP[tex_id];
}

const gfx_texture get_or_add_texture_from_string(const char *tex_key) {
  const unsigned int tex_id = get_tex_id_from_string(tex_key);
  return get_or_add_texture(tex_id);
}

void generate_textures() {
  TEXTURE_ASSET_REF[SQUARE] = (texture_asset){ "res/assets/textures/square.png", 512, 512 };
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
  for (unsigned int i = ELEMENT_GRASS_BUSH_DARK_0, count = 0; i <= ELEMENT_GRASS_BUSH_DARK_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/grass_bushes/dark/grass_bush_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_GRASS_BUSH_REG_0, count = 0; i <= ELEMENT_GRASS_BUSH_REG_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/grass_bushes/reg/grass_bush_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
  for (unsigned int i = ELEMENT_GRASS_MARKS_0, count = 0; i <= ELEMENT_GRASS_MARKS_59; i++) {
    char path[128];
    snprintf(path, sizeof(path), "res/assets/textures/tiles/elements/grass_marks/grass_marks_%d.png", count++);
    TEXTURE_ASSET_REF[i] = (texture_asset){ .tex_width = 336, .tex_height = 416 };
    strcpy(TEXTURE_ASSET_REF[i].tex_filepath, path);
  }
}

void reset_textures() {
  for (unsigned int i = 0; i < NUM_TEXTURES; i++) {
    TEXTURE_LOOKUP[i].id = NO_TEXTURE.id;
  }
}

