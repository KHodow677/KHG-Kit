#pragma once

#include "khg_gfx/texture.h"

typedef enum {
  TANK_BODY,
  TANK_TOP,
  PARTICLE_0,
  PARTICLE_1,
  PARTICLE_2,
  PARTICLE_3,
  PARTICLE_4,
  PARTICLE_5,
  PARTICLE_6,
  PARTICLE_7,
  PARTICLE_8,
  PARTICLE_9,
  PARTICLE_10,
  PARTICLE_11,
  PARTICLE_12,
  PARTICLE_13,
  PARTICLE_14,
  PARTICLE_15,
} TEXTURE_ID;

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height);
void add_new_texture(char *file_name, char *file_type, float width, float height);
void generate_textures(void);
void free_textures(void);

