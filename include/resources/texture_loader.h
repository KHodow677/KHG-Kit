#pragma once

#include "khg_gfx/texture.h"
#include <stdbool.h>

typedef enum {
  SQUARE,
  MAIN_ENVIRONMENT_GROUND,
  MAIN_ENVIRONMENT_TREE_0,
  MAIN_ENVIRONMENT_TREE_1,
  MAIN_ENVIRONMENT_MOUNTAIN_0,
  MAIN_ENVIRONMENT_MOUNTAIN_1,
  MAIN_ENVIRONMENT_BACKGROUND,
  NUM_TEXTURES
} TEXTURE_ID;

typedef struct texture_asset {
  gfx_texture tex;
  bool loaded;
  char *tex_file_name;
  char *tex_file_type;
  int tex_width;
  int tex_height;
} texture_asset;

extern texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height);
bool check_texture_loaded(int tex_id);
gfx_texture *get_or_add_texture(int tex_id);
void generate_textures(void);

