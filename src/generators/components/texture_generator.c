#include "generators/components/texture_generator.h"
#include "entity/ecs_manager.h"
#include "khg_gfx/texture.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

void add_new_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture *tank_body = malloc(sizeof(gfx_texture));
  *tank_body = generate_texture(file_name, file_type, width, height);
  utl_vector_push_back(TEXTURE_LOOKUP, &tank_body);
}

void generate_textures() {
  add_new_texture("Tank-Body-Blue", "png", 145, 184);
  add_new_texture("Tank-Top-Blue", "png", 102, 209);
  add_new_texture("particle/00", "png", 107, 100);
  add_new_texture("particle/01", "png", 107, 100);
  add_new_texture("particle/02", "png", 107, 100);
  add_new_texture("particle/03", "png", 107, 100);
  add_new_texture("particle/04", "png", 107, 100);
  add_new_texture("particle/05", "png", 107, 100);
  add_new_texture("particle/06", "png", 107, 100);
  add_new_texture("particle/07", "png", 107, 100);
  add_new_texture("particle/08", "png", 107, 100);
  add_new_texture("particle/09", "png", 107, 100);
  add_new_texture("particle/10", "png", 107, 100);
  add_new_texture("particle/11", "png", 107, 100);
  add_new_texture("particle/12", "png", 107, 100);
  add_new_texture("particle/13", "png", 107, 100);
  add_new_texture("particle/14", "png", 107, 100);
  add_new_texture("particle/15", "png", 107, 100);
}

void free_textures() {
  for (size_t i = 0; i < utl_vector_size(TEXTURE_LOOKUP); i++) {
    gfx_texture **tex = utl_vector_at(TEXTURE_LOOKUP, i);
    free(*tex);
  }
  utl_vector_deallocate(TEXTURE_LOOKUP);
}

