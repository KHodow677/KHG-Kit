#pragma once

#include "khg_gfx/texture.h"

typedef enum {
  SOIL_0,
  SOIL_1,
  SOIL_2,
  SOIL_3,
  BUILDING_0,
  BUILDING_1,
  BUILDING_2,
  BUILDING_3,
  BUILDING_4,
  BUILDING_5,
  BUILDING_6,
  BUILDING_7,
  BUILDING_8,
  BUILDING_9,
  BUILDING_10,
  BUILDING_11,
  BUILDING_12,
  PATH_0,
  PATH_1,
  PATH_2,
  PATH_3,
  PATH_4,
  PATH_5,
  HANGAR_ICON_BIG,
  TANK_BODY_ICON_SMALL,
  TANK_TOP_ICON_SMALL,
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
  TANK_BODY_OUTLINE,
  TANK_TOP_OUTLINE,
  COMMAND_POINT,
  COMMAND_LINE,
  TURRET_BASE,
  TURRET_TOP,
  HANGAR,
  HANGAR_OUTLINE,
  SLUG_1_0,
  SLUG_1_1,
  SLUG_1_2,
  SLUG_1_3,
  SLUG_1_4,
  SLUG_1_5,
  SLUG_1_6,
  SLUG_1_7,
  SLUG_1_8,
  SLUG_1_9,
  SLUG_1_10,
  SLUG_1_11,
  SLUG_1_12,
  SLUG_1_13,
  SLUG_1_14,
  SLUG_1_15,
  SLUG_1_16,
  SLUG_1_17,
  SLUG_1_18,
  SLUG_1_19,
  SLUG_1_20,
  SLUG_1_21,
  SLUG_1_22,
  SLUG_1_23,
  SLUG_1_24,
  SLUG_1_25,
  SLUG_1_26,
  SLUG_1_27,
  SLUG_1_28,
  NUM_TEXTURES
} TEXTURE_ID;

typedef struct texture_asset {
  char *tex_file_name;
  char *tex_file_type;
  int tex_width;
  int tex_height;
  int collision_direction;
} texture_asset;

extern texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height);
bool check_texture_loaded(int tex_id);
gfx_texture *get_or_add_texture(int tex_id);
void add_new_texture(int tex_id, char *file_name, char *file_type, float width, float height);
void generate_textures(void);
void free_textures(void);

