#pragma once

#include "khg_gfx/texture.h"

typedef enum {
  SQUARE,
  MAIN_ENVIRONMENT_GROUND,
  MAIN_ENVIRONMENT_TREE_0,
  MAIN_ENVIRONMENT_TREE_1,
  MAIN_ENVIRONMENT_MOUNTAIN_0,
  MAIN_ENVIRONMENT_MOUNTAIN_1,
  MAIN_ENVIRONMENT_BACKGROUND,
  MAIN_ENVIRONMENT_ROCK_0,
  MAIN_STATION_CART,
  MAIN_STATION_TWIN_HOUSE,
  PLAYER_IDLE_0,
  PLAYER_IDLE_1,
  PLAYER_IDLE_2,
  PLAYER_IDLE_3,
  PLAYER_IDLE_4,
  PLAYER_IDLE_5,
  PLAYER_IDLE_6,
  PLAYER_IDLE_7,
  ANIMAL_ELK_IDLE_0,
  ANIMAL_ELK_IDLE_1,
  ANIMAL_ELK_IDLE_2,
  ANIMAL_ELK_IDLE_3,
  ANIMAL_ELK_IDLE_4,
  ANIMAL_ELK_IDLE_5,
  ANIMAL_ELK_IDLE_6,
  ANIMAL_ELK_IDLE_7,
  ANIMAL_ELK_IDLE_8,
  ANIMAL_ELK_IDLE_9,
  ANIMAL_ELK_IDLE_10,
  ANIMAL_ELK_IDLE_11,
  ANIMAL_ELK_IDLE_12,
  ANIMAL_ELK_IDLE_13,
  ANIMAL_ELK_IDLE_14,
  ANIMAL_ELK_IDLE_15,
  ANIMAL_ELK_IDLE_16,
  ANIMAL_ELK_IDLE_17,
  ANIMAL_ELK_IDLE_18,
  ANIMAL_ELK_IDLE_19,
  ANIMAL_ELK_IDLE_20,
  ANIMAL_ELK_IDLE_21,
  ANIMAL_ELK_IDLE_22,
  ANIMAL_ELK_IDLE_23,
  ANIMAL_ELK_IDLE_24,
  ANIMAL_ELK_IDLE_25,
  ANIMAL_ELK_IDLE_26,
  ANIMAL_ELK_IDLE_27,
  ANIMAL_ELK_IDLE_28,
  ANIMAL_ELK_IDLE_29,
  PLAYER_HEAD,
  PLAYER_BODY,
  PLAYER_ARM_L,
  PLAYER_ARM_R,
  PLAYER_LEG_L,
  PLAYER_LEG_R,
  NUM_TEXTURES
} TEXTURE_ID;

typedef struct texture_asset {
  char *tex_file_name;
  char *tex_file_type;
  int tex_width;
  int tex_height;
  int collision_direction;
  int collision_shrinkage_offset;
} texture_asset;

const gfx_texture generate_texture(char *file_name, char *file_type, float width, float height);
const bool check_texture_loaded(int tex_id);
const gfx_texture get_or_add_texture(int tex_id);
void generate_textures(void);

