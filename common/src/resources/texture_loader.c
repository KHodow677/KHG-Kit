#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"
#include "khg_utl/algorithm.h"
#include <string.h>

static gfx_texture NO_TEXTURE = { 0 };
static gfx_texture TEXTURE_LOOKUP[NUM_TEXTURES];
static texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

static int compare_texture_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const gfx_texture generate_texture(char *filepath, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(filepath);
  tex.width = width;
  tex.height = height;
  tex.angle = 0;
  return tex;
}

const bool check_texture_loaded(unsigned int tex_id) {
  return (tex_id == NULL_TEXTURE || TEXTURE_LOOKUP[tex_id].id != NO_TEXTURE.id);
}

const unsigned int get_tex_id_from_string(const char *tex_key) {
  return utl_algorithm_find_at(TEXTURE_STRINGS, TEXTURE_STRINGS_SIZE, sizeof(char *), tex_key, compare_texture_strings);
}

const gfx_texture get_or_add_texture(unsigned int tex_id) {
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
  TEXTURE_ASSET_REF[ELEMENT_DIRTPATCH_6] = (texture_asset){ "res/assets/textures/tiles/elements/dirt_patches/dirt_patch_6.png", 336, 416 };
  for (unsigned int i = 0; i < NUM_TEXTURES; i++) {
    get_or_add_texture(i);
  }
}

void reset_textures() {
  for (unsigned int i = 0; i < NUM_TEXTURES; i++) {
    TEXTURE_LOOKUP[i].id = NO_TEXTURE.id;
  }
}

