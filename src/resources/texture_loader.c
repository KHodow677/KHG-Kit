#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"

texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

bool check_texture_loaded(int tex_id) {
  texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  return ta.loaded;
}

gfx_texture *get_or_add_texture(int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return &TEXTURE_ASSET_REF[tex_id].tex;
  }
  texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  gfx_texture *texture = &TEXTURE_ASSET_REF[tex_id].tex;
  *texture = generate_texture(ta.tex_file_name, ta.tex_file_type, ta.tex_width, ta.tex_height);
  return texture;
}

void generate_textures() {
  TEXTURE_ASSET_REF[SQUARE] = (texture_asset) { (gfx_texture){ 0 }, false, "square", "png", 256, 256};
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_GROUND] = (texture_asset) { (gfx_texture){ 0 }, false, "main/environment/ground_layer", "png", 1920, 906};
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_0] = (texture_asset) { (gfx_texture){ 0 }, false, "main/environment/tree_layer_0", "png", 1920, 441};
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_1] = (texture_asset) { (gfx_texture){ 0 }, false, "main/environment/tree_layer_1", "png", 1920, 612};
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_0] = (texture_asset) { (gfx_texture){ 0 }, false, "main/environment/mountain_layer_0", "png", 1920, 582};
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_1] = (texture_asset) { (gfx_texture){ 0 }, false, "main/environment/mountain_layer_1", "png", 1920, 567};
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_BACKGROUND] = (texture_asset) { (gfx_texture){ 0 }, false, "main/environment/background_layer", "png", 1920, 1080};
}

