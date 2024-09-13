#include "generators/components/texture_generator.h"
#include "game_manager.h"
#include "khg_gfx/texture.h"
#include "khg_utl/vector.h"
#include <stdio.h>

texture_asset TEXTURE_REF[NUM_TEXTURES];

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
  texture_asset ta = TEXTURE_REF[tex_id];
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
  TEXTURE_REF[GRASS_1] = (texture_asset){ "environment/Grass1", "png", 256, 256 };
  TEXTURE_REF[GRASS_2] = (texture_asset){ "environment/Grass2", "png", 256, 256 };
  TEXTURE_REF[TANK_BODY] = (texture_asset){ "Tank-Body-Blue", "png", 145, 184 };
  TEXTURE_REF[TANK_TOP] = (texture_asset){ "Tank-Top-Blue", "png", 102, 209 };
  /*add_new_texture(GRASS_1, "environment/Grass1", "png", 256, 256);*/
  /*add_new_texture(GRASS_2, "environment/Grass2", "png", 256, 256);*/
  /*add_new_texture(TANK_BODY, "Tank-Body-Blue", "png", 145, 184);*/
  /*add_new_texture(TANK_TOP, "Tank-Top-Blue", "png", 102, 209);*/
  add_new_texture(PARTICLE_1_0, "particle/p1/00", "png", 107, 100);
  add_new_texture(PARTICLE_1_1, "particle/p1/01", "png", 107, 100);
  add_new_texture(PARTICLE_1_2, "particle/p1/02", "png", 107, 100);
  add_new_texture(PARTICLE_1_3, "particle/p1/03", "png", 107, 100);
  add_new_texture(PARTICLE_1_4, "particle/p1/04", "png", 107, 100);
  add_new_texture(PARTICLE_1_5, "particle/p1/05", "png", 107, 100);
  add_new_texture(PARTICLE_1_6, "particle/p1/06", "png", 107, 100);
  add_new_texture(PARTICLE_1_7, "particle/p1/07", "png", 107, 100);
  add_new_texture(PARTICLE_1_8, "particle/p1/08", "png", 107, 100);
  add_new_texture(PARTICLE_1_9, "particle/p1/09", "png", 107, 100);
  add_new_texture(PARTICLE_1_10, "particle/p1/10", "png", 107, 100);
  add_new_texture(PARTICLE_1_11, "particle/p1/11", "png", 107, 100);
  add_new_texture(PARTICLE_1_12, "particle/p1/12", "png", 107, 100);
  add_new_texture(PARTICLE_1_13, "particle/p1/13", "png", 107, 100);
  add_new_texture(PARTICLE_1_14, "particle/p1/14", "png", 107, 100);
  add_new_texture(PARTICLE_1_15, "particle/p1/15", "png", 107, 100);
  add_new_texture(PARTICLE_2_0, "particle/p2/00", "png", 80, 80);
  add_new_texture(PARTICLE_2_1, "particle/p2/01", "png", 80, 80);
  add_new_texture(PARTICLE_2_2, "particle/p2/02", "png", 80, 80);
  add_new_texture(PARTICLE_2_3, "particle/p2/03", "png", 80, 80);
  add_new_texture(PARTICLE_2_4, "particle/p2/04", "png", 80, 80);
  add_new_texture(TANK_BODY_OUTLINE, "Tank-Body-Outline", "png", 154, 192);
  add_new_texture(TANK_TOP_OUTLINE, "Tank-Top-Outline", "png", 111, 217);
  add_new_texture(COMMAND_POINT, "targeting/Point", "png", 56, 56);
  /*add_new_texture(COMMAND_LINE, "targeting/Line", "png", 20, 1);*/
  printf("LOADED: %i\n", check_texture_loaded(COMMAND_POINT));
}

void free_textures() {
  utl_vector_deallocate(TEXTURE_LOOKUP);
}
