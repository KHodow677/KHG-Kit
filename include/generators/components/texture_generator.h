#pragma once

#include "khg_gfx/texture.h"

typedef enum {
  TANK_BODY,
  TANK_TOP,
  PARTICLE_0,
  PARTICLE_1,
} TEXTURE_ID;

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height);
void add_new_texture(char *file_name, char *file_type, float width, float height);
void generate_textures(void);
void free_textures(void);

