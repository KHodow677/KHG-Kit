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
  PLAYER_IDLE_8,
  PLAYER_IDLE_9,
  PLAYER_IDLE_10,
  PLAYER_IDLE_11,
  PLAYER_IDLE_12,
  PLAYER_IDLE_13,
  PLAYER_IDLE_14,
  PLAYER_IDLE_15,
  PLAYER_IDLE_16,
  PLAYER_IDLE_17,
  PLAYER_IDLE_18,
  PLAYER_IDLE_19,
  PLAYER_IDLE_20,
  PLAYER_IDLE_21,
  PLAYER_IDLE_22,
  PLAYER_IDLE_23,
  PLAYER_IDLE_24,
  PLAYER_IDLE_25,
  PLAYER_IDLE_26,
  PLAYER_IDLE_27,
  PLAYER_IDLE_28,
  PLAYER_IDLE_29,
  PLAYER_IDLE_30,
  PLAYER_IDLE_31,
  PLAYER_IDLE_32,
  PLAYER_IDLE_33,
  PLAYER_IDLE_34,
  PLAYER_IDLE_35,
  PLAYER_IDLE_36,
  PLAYER_IDLE_37,
  PLAYER_IDLE_38,
  PLAYER_IDLE_39,
  PLAYER_IDLE_40,
  PLAYER_IDLE_41,
  PLAYER_IDLE_42,
  PLAYER_IDLE_43,
  PLAYER_IDLE_44,
  PLAYER_IDLE_45,
  PLAYER_IDLE_46,
  PLAYER_IDLE_47,
  PLAYER_IDLE_48,
  PLAYER_IDLE_49,
  PLAYER_IDLE_50,
  PLAYER_IDLE_51,
  PLAYER_IDLE_52,
  PLAYER_IDLE_53,
  PLAYER_IDLE_54,
  PLAYER_IDLE_55,
  PLAYER_IDLE_56,
  PLAYER_IDLE_57,
  PLAYER_IDLE_58,
  PLAYER_IDLE_59,
  PLAYER_WALK_0,
  PLAYER_WALK_1,
  PLAYER_WALK_2,
  PLAYER_WALK_3,
  PLAYER_WALK_4,
  PLAYER_WALK_5,
  PLAYER_WALK_6,
  PLAYER_WALK_7,
  PLAYER_WALK_8,
  PLAYER_WALK_9,
  PLAYER_WALK_10,
  PLAYER_WALK_11,
  PLAYER_WALK_12,
  PLAYER_WALK_13,
  PLAYER_WALK_14,
  PLAYER_WALK_15,
  PLAYER_WALK_16,
  PLAYER_WALK_17,
  PLAYER_WALK_18,
  PLAYER_WALK_19,
  PLAYER_WALK_20,
  PLAYER_WALK_21,
  PLAYER_WALK_22,
  PLAYER_WALK_23,
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

