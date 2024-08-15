#pragma once

#include "khg_gfx/texture.h"

typedef enum {
  TANK_BODY,
  TANK_TOP,
  PARTICLE_1_0,
  PARTICLE_1_1,
  PARTICLE_1_2,
  PARTICLE_1_3,
  PARTICLE_1_4,
  PARTICLE_1_5,
  PARTICLE_1_6,
  PARTICLE_1_7,
  PARTICLE_1_8,
  PARTICLE_1_9,
  PARTICLE_1_10,
  PARTICLE_1_11,
  PARTICLE_1_12,
  PARTICLE_1_13,
  PARTICLE_1_14,
  PARTICLE_1_15,
  PARTICLE_2_0,
  PARTICLE_2_1,
  PARTICLE_2_2,
  PARTICLE_2_3,
  PARTICLE_2_4,
} TEXTURE_ID;

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height);
void add_new_texture(char *file_name, char *file_type, float width, float height);
void generate_textures(void);
void free_textures(void);

