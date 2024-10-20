#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"

static gfx_texture NO_TEXTURE = { 0 };
static gfx_texture TEXTURE_LOOKUP[NUM_TEXTURES];
static texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

bool check_texture_loaded(int tex_id) {
  return (TEXTURE_LOOKUP[tex_id].id != NO_TEXTURE.id);
}

gfx_texture get_or_add_texture(int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return TEXTURE_LOOKUP[tex_id];
  }
  texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  TEXTURE_LOOKUP[tex_id] = generate_texture(ta.tex_file_name, ta.tex_file_type, ta.tex_width, ta.tex_height);
  return TEXTURE_LOOKUP[tex_id];
}

void generate_textures() {
  TEXTURE_ASSET_REF[SQUARE] = (texture_asset){ "square", "png", 512, 512, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_GROUND] = (texture_asset){ "main/environment/ground_layer", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_0] = (texture_asset){ "main/environment/tree_layer_00", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_1] = (texture_asset){ "main/environment/tree_layer_01", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_0] = (texture_asset){ "main/environment/mountain_layer_00", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_1] = (texture_asset){ "main/environment/mountain_layer_01", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_BACKGROUND] = (texture_asset){ "main/environment/background_layer", "png", 1282, 722, 0 };
}

