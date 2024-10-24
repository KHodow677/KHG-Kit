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
  PLAYER_WALK_00,
  PLAYER_WALK_01,
  PLAYER_WALK_02,
  PLAYER_WALK_03,
  PLAYER_WALK_04,
  PLAYER_WALK_05,
  PLAYER_WALK_06,
  PLAYER_WALK_07,
  PLAYER_WALK_08,
  PLAYER_WALK_09,
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

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height);
bool check_texture_loaded(int tex_id);
gfx_texture get_or_add_texture(int tex_id);
void generate_textures(void);

