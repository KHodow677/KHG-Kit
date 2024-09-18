#include "generators/components/texture_generator.h"
#include "game_manager.h"
#include "khg_gfx/texture.h"
#include "khg_utl/vector.h"

texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

bool check_texture_loaded(int tex_id) {
  gfx_texture *texture = utl_vector_at(TEXTURE_LOOKUP, tex_id);
  return (texture->id != NO_TEXTURE.id);
}

gfx_texture *get_or_add_texture(int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return utl_vector_at(TEXTURE_LOOKUP, tex_id);
  }
  texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  gfx_texture *texture = utl_vector_at(TEXTURE_LOOKUP, tex_id);
  *texture = generate_texture(ta.tex_file_name, ta.tex_file_type, ta.tex_width, ta.tex_height);
  return texture;
}

void add_new_texture(int tex_id, char *file_name, char *file_type, float width, float height) {
  gfx_texture *texture = utl_vector_at(TEXTURE_LOOKUP, tex_id);
  *texture = generate_texture(file_name, file_type, width, height);
}

void generate_textures() {
  TEXTURE_LOOKUP = utl_vector_create(sizeof(gfx_texture));
  for (int i = 0; i < MAX_TEXTURES; i++) {
    utl_vector_push_back(TEXTURE_LOOKUP, &NO_TEXTURE);
  }
  TEXTURE_ASSET_REF[GRASS_1] = (texture_asset){ "environment/Grass1", "png", 264, 264 };
  TEXTURE_ASSET_REF[GRASS_2] = (texture_asset){ "environment/Grass2", "png", 264, 264 };
  TEXTURE_ASSET_REF[TANK_BODY] = (texture_asset){ "Tank-Body-Blue", "png", 145, 184 };
  TEXTURE_ASSET_REF[TANK_TOP] = (texture_asset){ "Tank-Top-Blue", "png", 102, 209 };
  TEXTURE_ASSET_REF[PARTICLE_1_0] = (texture_asset){ "particle/p1/00", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_1] = (texture_asset){ "particle/p1/01", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_2] = (texture_asset){ "particle/p1/02", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_3] = (texture_asset){ "particle/p1/03", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_4] = (texture_asset){ "particle/p1/04", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_5] = (texture_asset){ "particle/p1/05", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_6] = (texture_asset){ "particle/p1/06", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_7] = (texture_asset){ "particle/p1/07", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_8] = (texture_asset){ "particle/p1/08", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_9] = (texture_asset){ "particle/p1/09", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_10] = (texture_asset){  "particle/p1/10", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_11] = (texture_asset){  "particle/p1/11", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_12] = (texture_asset){  "particle/p1/12", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_13] = (texture_asset){  "particle/p1/13", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_14] = (texture_asset){ "particle/p1/14", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_1_15] = (texture_asset){ "particle/p1/15", "png", 107, 100 };
  TEXTURE_ASSET_REF[PARTICLE_2_0] = (texture_asset){ "particle/p2/00", "png", 80, 80 };
  TEXTURE_ASSET_REF[PARTICLE_2_1] = (texture_asset){ "particle/p2/01", "png", 80, 80 };
  TEXTURE_ASSET_REF[PARTICLE_2_2] = (texture_asset){ "particle/p2/02", "png", 80, 80 };
  TEXTURE_ASSET_REF[PARTICLE_2_3] = (texture_asset){ "particle/p2/03", "png", 80, 80 };
  TEXTURE_ASSET_REF[PARTICLE_2_4] = (texture_asset){ "particle/p2/04", "png", 80, 80 };
  TEXTURE_ASSET_REF[TANK_BODY_OUTLINE] = (texture_asset){ "Tank-Body-Outline", "png", 154, 192 };
  TEXTURE_ASSET_REF[TANK_TOP_OUTLINE] = (texture_asset){ "Tank-Top-Outline", "png", 111, 217 };
  TEXTURE_ASSET_REF[COMMAND_POINT] = (texture_asset){ "targeting/Point", "png", 56, 56 };
  TEXTURE_ASSET_REF[COMMAND_LINE] = (texture_asset){ "targeting/Line", "png", 20, 1 };
}

void free_textures() {
  utl_vector_deallocate(TEXTURE_LOOKUP);
}
