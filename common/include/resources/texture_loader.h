#pragma once

#include "khg_gfx/texture.h"

#define FOREACH_TEXTURE(TEXTURE) \
  TEXTURE(NULL_TEXTURE)\
  TEXTURE(SQUARE)\
  TEXTURE(GROUND_GRASS)\
  TEXTURE(BORDER_BLACK)\
  TEXTURE(ELEMENT_BUSH_0) TEXTURE(ELEMENT_BUSH_1) TEXTURE(ELEMENT_BUSH_2) TEXTURE(ELEMENT_BUSH_3) TEXTURE(ELEMENT_BUSH_4) TEXTURE(ELEMENT_BUSH_5) TEXTURE(ELEMENT_BUSH_6) TEXTURE(ELEMENT_BUSH_7) TEXTURE(ELEMENT_BUSH_8) TEXTURE(ELEMENT_BUSH_9)\
  TEXTURE(ELEMENT_BUSH_10) TEXTURE(ELEMENT_BUSH_11) TEXTURE(ELEMENT_BUSH_12) TEXTURE(ELEMENT_BUSH_13) TEXTURE(ELEMENT_BUSH_14) TEXTURE(ELEMENT_BUSH_15) TEXTURE(ELEMENT_BUSH_16) TEXTURE(ELEMENT_BUSH_17) TEXTURE(ELEMENT_BUSH_18) TEXTURE(ELEMENT_BUSH_19)\
  TEXTURE(ELEMENT_BUSH_20) TEXTURE(ELEMENT_BUSH_21) TEXTURE(ELEMENT_BUSH_22) TEXTURE(ELEMENT_BUSH_23) TEXTURE(ELEMENT_BUSH_24) TEXTURE(ELEMENT_BUSH_25) TEXTURE(ELEMENT_BUSH_26) TEXTURE(ELEMENT_BUSH_27) TEXTURE(ELEMENT_BUSH_28) TEXTURE(ELEMENT_BUSH_29)\
  TEXTURE(ELEMENT_BUSH_30) TEXTURE(ELEMENT_BUSH_31) TEXTURE(ELEMENT_BUSH_32) TEXTURE(ELEMENT_BUSH_33) TEXTURE(ELEMENT_BUSH_34) TEXTURE(ELEMENT_BUSH_35) TEXTURE(ELEMENT_BUSH_36) TEXTURE(ELEMENT_BUSH_37) TEXTURE(ELEMENT_BUSH_38) TEXTURE(ELEMENT_BUSH_39)\
  TEXTURE(ELEMENT_BUSH_40) TEXTURE(ELEMENT_BUSH_41) TEXTURE(ELEMENT_BUSH_42) TEXTURE(ELEMENT_BUSH_43) TEXTURE(ELEMENT_BUSH_44) TEXTURE(ELEMENT_BUSH_45) TEXTURE(ELEMENT_BUSH_46) TEXTURE(ELEMENT_BUSH_47) TEXTURE(ELEMENT_BUSH_48) TEXTURE(ELEMENT_BUSH_49)\
  TEXTURE(ELEMENT_BUSH_50) TEXTURE(ELEMENT_BUSH_51) TEXTURE(ELEMENT_BUSH_52) TEXTURE(ELEMENT_BUSH_53) TEXTURE(ELEMENT_BUSH_54) TEXTURE(ELEMENT_BUSH_55) TEXTURE(ELEMENT_BUSH_56) TEXTURE(ELEMENT_BUSH_57) TEXTURE(ELEMENT_BUSH_58) TEXTURE(ELEMENT_BUSH_59)\
  TEXTURE(ELEMENT_DIRTPATCH_0) TEXTURE(ELEMENT_DIRTPATCH_1) TEXTURE(ELEMENT_DIRTPATCH_2) TEXTURE(ELEMENT_DIRTPATCH_3) TEXTURE(ELEMENT_DIRTPATCH_4) TEXTURE(ELEMENT_DIRTPATCH_5) TEXTURE(ELEMENT_DIRTPATCH_6) TEXTURE(ELEMENT_DIRTPATCH_7) TEXTURE(ELEMENT_DIRTPATCH_8) TEXTURE(ELEMENT_DIRTPATCH_9)\
  TEXTURE(ELEMENT_DIRTPATCH_10) TEXTURE(ELEMENT_DIRTPATCH_11) TEXTURE(ELEMENT_DIRTPATCH_12) TEXTURE(ELEMENT_DIRTPATCH_13) TEXTURE(ELEMENT_DIRTPATCH_14) TEXTURE(ELEMENT_DIRTPATCH_15) TEXTURE(ELEMENT_DIRTPATCH_16) TEXTURE(ELEMENT_DIRTPATCH_17) TEXTURE(ELEMENT_DIRTPATCH_18) TEXTURE(ELEMENT_DIRTPATCH_19)\
  TEXTURE(ELEMENT_DIRTPATCH_20) TEXTURE(ELEMENT_DIRTPATCH_21) TEXTURE(ELEMENT_DIRTPATCH_22) TEXTURE(ELEMENT_DIRTPATCH_23) TEXTURE(ELEMENT_DIRTPATCH_24) TEXTURE(ELEMENT_DIRTPATCH_25) TEXTURE(ELEMENT_DIRTPATCH_26) TEXTURE(ELEMENT_DIRTPATCH_27) TEXTURE(ELEMENT_DIRTPATCH_28) TEXTURE(ELEMENT_DIRTPATCH_29)\
  TEXTURE(ELEMENT_DIRTPATCH_30) TEXTURE(ELEMENT_DIRTPATCH_31) TEXTURE(ELEMENT_DIRTPATCH_32) TEXTURE(ELEMENT_DIRTPATCH_33) TEXTURE(ELEMENT_DIRTPATCH_34) TEXTURE(ELEMENT_DIRTPATCH_35) TEXTURE(ELEMENT_DIRTPATCH_36) TEXTURE(ELEMENT_DIRTPATCH_37) TEXTURE(ELEMENT_DIRTPATCH_38) TEXTURE(ELEMENT_DIRTPATCH_39)\
  TEXTURE(ELEMENT_DIRTPATCH_40) TEXTURE(ELEMENT_DIRTPATCH_41) TEXTURE(ELEMENT_DIRTPATCH_42) TEXTURE(ELEMENT_DIRTPATCH_43) TEXTURE(ELEMENT_DIRTPATCH_44) TEXTURE(ELEMENT_DIRTPATCH_45) TEXTURE(ELEMENT_DIRTPATCH_46) TEXTURE(ELEMENT_DIRTPATCH_47) TEXTURE(ELEMENT_DIRTPATCH_48) TEXTURE(ELEMENT_DIRTPATCH_49)\
  TEXTURE(ELEMENT_DIRTPATCH_50) TEXTURE(ELEMENT_DIRTPATCH_51) TEXTURE(ELEMENT_DIRTPATCH_52) TEXTURE(ELEMENT_DIRTPATCH_53) TEXTURE(ELEMENT_DIRTPATCH_54) TEXTURE(ELEMENT_DIRTPATCH_55) TEXTURE(ELEMENT_DIRTPATCH_56) TEXTURE(ELEMENT_DIRTPATCH_57) TEXTURE(ELEMENT_DIRTPATCH_58) TEXTURE(ELEMENT_DIRTPATCH_59)\
  TEXTURE(NUM_TEXTURES)

#define GENERATE_TEXTURE_ENUM(ENUM) ENUM,
#define GENERATE_TEXTURE_STRING(STRING) #STRING,

typedef enum {
  FOREACH_TEXTURE(GENERATE_TEXTURE_ENUM)
} texture_id;

#define TEXTURE_STRINGS (char *[]){ FOREACH_TEXTURE(GENERATE_TEXTURE_STRING) }
#define TEXTURE_STRINGS_SIZE sizeof(TEXTURE_STRINGS) / sizeof(TEXTURE_STRINGS[0])

typedef struct texture_asset {
  char tex_filepath[128];
  int tex_width;
  int tex_height;
} texture_asset;

const gfx_texture generate_texture(const char *filepath, const float width, const float height);
const bool check_texture_loaded(const unsigned int tex_id);
const unsigned int get_tex_id_from_string(const char *tex_key);

const gfx_texture get_or_add_texture(const unsigned int tex_id);
const gfx_texture get_or_add_texture_from_string(const char *tex_key);

void generate_textures(void);
void reset_textures(void);

