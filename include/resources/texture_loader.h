#pragma once

#include "khg_gfx/texture.h"

#define FOREACH_TEXTURE(TEXTURE)\
  TEXTURE(SQUARE)\
  TEXTURE(MAIN_ENVIRONMENT_GROUND)\
  TEXTURE(MAIN_ENVIRONMENT_TREE_0)\
  TEXTURE(MAIN_ENVIRONMENT_TREE_1)\
  TEXTURE(MAIN_ENVIRONMENT_MOUNTAIN_0)\
  TEXTURE(MAIN_ENVIRONMENT_MOUNTAIN_1)\
  TEXTURE(MAIN_ENVIRONMENT_BACKGROUND)\
  TEXTURE(MAIN_ENVIRONMENT_ROCK_0)\
  TEXTURE(MAIN_STATION_CART)\
  TEXTURE(MAIN_STATION_TWIN_HOUSE)\
  TEXTURE(PLAYER_HEAD)\
  TEXTURE(PLAYER_BODY)\
  TEXTURE(PLAYER_ARM_L)\
  TEXTURE(PLAYER_ARM_R)\
  TEXTURE(PLAYER_LEG_L)\
  TEXTURE(PLAYER_LEG_R)\
  TEXTURE(TILE_DUNGEON_0)\
  TEXTURE(TILE_DUNGEON_1)\
  TEXTURE(TILE_DUNGEON_2)\
  TEXTURE(TILE_DUNGEON_3)\
  TEXTURE(TILE_DUNGEON_4)\
  TEXTURE(TILE_DUNGEON_5)\
  TEXTURE(TILE_DUNGEON_6)\
  TEXTURE(TILE_DUNGEON_7)\
  TEXTURE(TILE_DUNGEON_8)\
  TEXTURE(TILE_DUNGEON_9)\
  TEXTURE(TILE_DUNGEON_10)\
  TEXTURE(TILE_DUNGEON_11)\
  TEXTURE(TILE_DUNGEON_12)\
  TEXTURE(TILE_DUNGEON_13)\
  TEXTURE(TILE_DUNGEON_14)\
  TEXTURE(TILE_DUNGEON_15)\
  TEXTURE(TILE_DUNGEON_16)\
  TEXTURE(TILE_DUNGEON_17)\
  TEXTURE(TILE_DUNGEON_18)\
  TEXTURE(TILE_DUNGEON_19)\
  TEXTURE(TILE_DUNGEON_20)\
  TEXTURE(TILE_DUNGEON_21)\
  TEXTURE(TILE_DUNGEON_22)\
  TEXTURE(TILE_DUNGEON_23)\
  TEXTURE(TILE_DUNGEON_24)\
  TEXTURE(TILE_DUNGEON_25)\
  TEXTURE(TILE_DUNGEON_26)\
  TEXTURE(TILE_DUNGEON_27)\
  TEXTURE(TILE_DUNGEON_28)\
  TEXTURE(TILE_DUNGEON_29)\
  TEXTURE(TILE_DUNGEON_30)\
  TEXTURE(TILE_DUNGEON_31)\
  TEXTURE(TILE_DUNGEON_32)\
  TEXTURE(TILE_DUNGEON_33)\
  TEXTURE(TILE_DUNGEON_34)\
  TEXTURE(TILE_DUNGEON_35)\
  TEXTURE(TILE_DUNGEON_36)\
  TEXTURE(TILE_DUNGEON_37)\
  TEXTURE(TILE_DUNGEON_38)\
  TEXTURE(TILE_DUNGEON_39)\
  TEXTURE(NUM_TEXTURES)\

#define GENERATE_TEXTURE_ENUM(ENUM) ENUM,
#define GENERATE_TEXTURE_STRING(STRING) #STRING,

typedef enum {
  FOREACH_TEXTURE(GENERATE_TEXTURE_ENUM)
} texture_id;

#define TEXTURE_STRINGS (char *[]){ FOREACH_TEXTURE(GENERATE_TEXTURE_STRING) }
#define TEXTURE_STRINGS_SIZE sizeof(TEXTURE_STRINGS) / sizeof(TEXTURE_STRINGS[0])

typedef struct texture_asset {
  char *tex_filepath;
  int tex_width;
  int tex_height;
} texture_asset;

const gfx_texture generate_texture(char *filepath, float width, float height);
const bool check_texture_loaded(int tex_id);
const int get_tex_id_from_string(const char *tex_key);
const gfx_texture get_or_add_texture(int tex_id);
const gfx_texture get_or_add_texture_from_string(const char *tex_key);
void generate_textures(void);
void reset_textures(void);

