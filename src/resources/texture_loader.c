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
}
