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

