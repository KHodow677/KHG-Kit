#pragma once

#include "khg_gfx/texture.h"
#include "khg_utl/map.h"

typedef struct util_texture_asset {
  char tex_filepath[128];
  int tex_width;
  int tex_height;
} util_texture_asset;

typedef struct config_texture_raw_info {
  unsigned char *tex_raw;
  int width;
  int height;
  int channels;
} config_texture_raw_info;

extern utl_map *TEXTURE_ASSETS;
extern utl_map *TILE_ASSETS;

void generate_tex_defs(const char *filename);
void emplace_tex_defs_tick(utl_map_iterator *map_it);
void emplace_tex_defs(void);
gfx_texture get_tex_def(char *tex_str);
void free_tex_defs(void);

void generate_tile_defs(const char *filename);
void free_tile_defs(void);

