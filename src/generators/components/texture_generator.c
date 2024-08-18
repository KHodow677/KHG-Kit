#include "generators/components/texture_generator.h"
#include "entity/ecs_manager.h"
#include "khg_gfx/texture.h"
#include "khg_utl/vector.h"
#include <stdio.h>

int get_current_texture() {
  return CURRENT_TEXTURE_ID++; 
}

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

void add_new_texture(char *file_name, char *file_type, float width, float height) {
  int id = get_current_texture();
  gfx_texture *texture = utl_vector_at(TEXTURE_LOOKUP, id);
  *texture = generate_texture(file_name, file_type, width, height);
}

void generate_textures() {
  TEXTURE_LOOKUP = utl_vector_create(sizeof(gfx_texture));
  for (int i = 0; i < MAX_TEXTURES; i++) {
    utl_vector_push_back(TEXTURE_LOOKUP, &NO_TEXTURE);
  }
  add_new_texture("Tank-Body-Blue", "png", 145, 184);
  add_new_texture("Tank-Top-Blue", "png", 102, 209);
  add_new_texture("particle/p1/00", "png", 107, 100);
  add_new_texture("particle/p1/01", "png", 107, 100);
  add_new_texture("particle/p1/02", "png", 107, 100);
  add_new_texture("particle/p1/03", "png", 107, 100);
  add_new_texture("particle/p1/04", "png", 107, 100);
  add_new_texture("particle/p1/05", "png", 107, 100);
  add_new_texture("particle/p1/06", "png", 107, 100);
  add_new_texture("particle/p1/07", "png", 107, 100);
  add_new_texture("particle/p1/08", "png", 107, 100);
  add_new_texture("particle/p1/09", "png", 107, 100);
  add_new_texture("particle/p1/10", "png", 107, 100);
  add_new_texture("particle/p1/11", "png", 107, 100);
  add_new_texture("particle/p1/12", "png", 107, 100);
  add_new_texture("particle/p1/13", "png", 107, 100);
  add_new_texture("particle/p1/14", "png", 107, 100);
  add_new_texture("particle/p1/15", "png", 107, 100);
  add_new_texture("particle/p2/00", "png", 80, 80);
  add_new_texture("particle/p2/01", "png", 80, 80);
  add_new_texture("particle/p2/02", "png", 80, 80);
  add_new_texture("particle/p2/03", "png", 80, 80);
  add_new_texture("particle/p2/04", "png", 80, 80);
}

void free_textures() {
  utl_vector_deallocate(TEXTURE_LOOKUP);
}

