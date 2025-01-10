#pragma once

#include "khg_gfx/texture.h"

#define FOREACH_TEXTURE(TEXTURE)\
  TEXTURE(NULL_TEXTURE)\
  TEXTURE(SQUARE)\
  TEXTURE(GROUND_GRASS)\
  TEXTURE(BORDER_BLACK)\
  TEXTURE(ELEMENT_DIRTPATCH_6)\
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
const bool check_texture_loaded(unsigned int tex_id);
const unsigned int get_tex_id_from_string(const char *tex_key);

const gfx_texture get_or_add_texture(unsigned int tex_id);
const gfx_texture get_or_add_texture_from_string(const char *tex_key);

void generate_textures(void);
void reset_textures(void);

