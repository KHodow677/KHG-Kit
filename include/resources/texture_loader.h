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
  TEXTURE(PLAYER_IDLE_0)\
  TEXTURE(PLAYER_IDLE_1)\
  TEXTURE(PLAYER_IDLE_2)\
  TEXTURE(PLAYER_IDLE_3)\
  TEXTURE(PLAYER_IDLE_4)\
  TEXTURE(PLAYER_IDLE_5)\
  TEXTURE(PLAYER_IDLE_6)\
  TEXTURE(PLAYER_IDLE_7)\
  TEXTURE(ANIMAL_ELK_IDLE_0)\
  TEXTURE(ANIMAL_ELK_IDLE_1)\
  TEXTURE(ANIMAL_ELK_IDLE_2)\
  TEXTURE(ANIMAL_ELK_IDLE_3)\
  TEXTURE(ANIMAL_ELK_IDLE_4)\
  TEXTURE(ANIMAL_ELK_IDLE_5)\
  TEXTURE(ANIMAL_ELK_IDLE_6)\
  TEXTURE(ANIMAL_ELK_IDLE_7)\
  TEXTURE(ANIMAL_ELK_IDLE_8)\
  TEXTURE(ANIMAL_ELK_IDLE_9)\
  TEXTURE(ANIMAL_ELK_IDLE_10)\
  TEXTURE(ANIMAL_ELK_IDLE_11)\
  TEXTURE(ANIMAL_ELK_IDLE_12)\
  TEXTURE(ANIMAL_ELK_IDLE_13)\
  TEXTURE(ANIMAL_ELK_IDLE_14)\
  TEXTURE(ANIMAL_ELK_IDLE_15)\
  TEXTURE(ANIMAL_ELK_IDLE_16)\
  TEXTURE(ANIMAL_ELK_IDLE_17)\
  TEXTURE(ANIMAL_ELK_IDLE_18)\
  TEXTURE(ANIMAL_ELK_IDLE_19)\
  TEXTURE(ANIMAL_ELK_IDLE_20)\
  TEXTURE(ANIMAL_ELK_IDLE_21)\
  TEXTURE(ANIMAL_ELK_IDLE_22)\
  TEXTURE(ANIMAL_ELK_IDLE_23)\
  TEXTURE(ANIMAL_ELK_IDLE_24)\
  TEXTURE(ANIMAL_ELK_IDLE_25)\
  TEXTURE(ANIMAL_ELK_IDLE_26)\
  TEXTURE(ANIMAL_ELK_IDLE_27)\
  TEXTURE(ANIMAL_ELK_IDLE_28)\
  TEXTURE(ANIMAL_ELK_IDLE_29)\
  TEXTURE(PLAYER_HEAD)\
  TEXTURE(PLAYER_BODY)\
  TEXTURE(PLAYER_ARM_L)\
  TEXTURE(PLAYER_ARM_R)\
  TEXTURE(PLAYER_LEG_L)\
  TEXTURE(PLAYER_LEG_R)\
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
