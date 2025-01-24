#pragma once

#include "khg_gfx/texture.h"

#define FOREACH_TEXTURE(TEXTURE)\
  TEXTURE(EMPTY_TEXTURE)\
  TEXTURE(GROUND_GRASS_GREEN)\
  TEXTURE(GROUND_GRASS_BLUE)\
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
  TEXTURE(ELEMENT_GRASSBUSH_DARK_0) TEXTURE(ELEMENT_GRASSBUSH_DARK_1) TEXTURE(ELEMENT_GRASSBUSH_DARK_2) TEXTURE(ELEMENT_GRASSBUSH_DARK_3) TEXTURE(ELEMENT_GRASSBUSH_DARK_4) TEXTURE(ELEMENT_GRASSBUSH_DARK_5) TEXTURE(ELEMENT_GRASSBUSH_DARK_6) TEXTURE(ELEMENT_GRASSBUSH_DARK_7) TEXTURE(ELEMENT_GRASSBUSH_DARK_8) TEXTURE(ELEMENT_GRASSBUSH_DARK_9)\
  TEXTURE(ELEMENT_GRASSBUSH_DARK_10) TEXTURE(ELEMENT_GRASSBUSH_DARK_11) TEXTURE(ELEMENT_GRASSBUSH_DARK_12) TEXTURE(ELEMENT_GRASSBUSH_DARK_13) TEXTURE(ELEMENT_GRASSBUSH_DARK_14) TEXTURE(ELEMENT_GRASSBUSH_DARK_15) TEXTURE(ELEMENT_GRASSBUSH_DARK_16) TEXTURE(ELEMENT_GRASSBUSH_DARK_17) TEXTURE(ELEMENT_GRASSBUSH_DARK_18) TEXTURE(ELEMENT_GRASSBUSH_DARK_19)\
  TEXTURE(ELEMENT_GRASSBUSH_DARK_20) TEXTURE(ELEMENT_GRASSBUSH_DARK_21) TEXTURE(ELEMENT_GRASSBUSH_DARK_22) TEXTURE(ELEMENT_GRASSBUSH_DARK_23) TEXTURE(ELEMENT_GRASSBUSH_DARK_24) TEXTURE(ELEMENT_GRASSBUSH_DARK_25) TEXTURE(ELEMENT_GRASSBUSH_DARK_26) TEXTURE(ELEMENT_GRASSBUSH_DARK_27) TEXTURE(ELEMENT_GRASSBUSH_DARK_28) TEXTURE(ELEMENT_GRASSBUSH_DARK_29)\
  TEXTURE(ELEMENT_GRASSBUSH_DARK_30) TEXTURE(ELEMENT_GRASSBUSH_DARK_31) TEXTURE(ELEMENT_GRASSBUSH_DARK_32) TEXTURE(ELEMENT_GRASSBUSH_DARK_33) TEXTURE(ELEMENT_GRASSBUSH_DARK_34) TEXTURE(ELEMENT_GRASSBUSH_DARK_35) TEXTURE(ELEMENT_GRASSBUSH_DARK_36) TEXTURE(ELEMENT_GRASSBUSH_DARK_37) TEXTURE(ELEMENT_GRASSBUSH_DARK_38) TEXTURE(ELEMENT_GRASSBUSH_DARK_39)\
  TEXTURE(ELEMENT_GRASSBUSH_DARK_40) TEXTURE(ELEMENT_GRASSBUSH_DARK_41) TEXTURE(ELEMENT_GRASSBUSH_DARK_42) TEXTURE(ELEMENT_GRASSBUSH_DARK_43) TEXTURE(ELEMENT_GRASSBUSH_DARK_44) TEXTURE(ELEMENT_GRASSBUSH_DARK_45) TEXTURE(ELEMENT_GRASSBUSH_DARK_46) TEXTURE(ELEMENT_GRASSBUSH_DARK_47) TEXTURE(ELEMENT_GRASSBUSH_DARK_48) TEXTURE(ELEMENT_GRASSBUSH_DARK_49)\
  TEXTURE(ELEMENT_GRASSBUSH_DARK_50) TEXTURE(ELEMENT_GRASSBUSH_DARK_51) TEXTURE(ELEMENT_GRASSBUSH_DARK_52) TEXTURE(ELEMENT_GRASSBUSH_DARK_53) TEXTURE(ELEMENT_GRASSBUSH_DARK_54) TEXTURE(ELEMENT_GRASSBUSH_DARK_55) TEXTURE(ELEMENT_GRASSBUSH_DARK_56) TEXTURE(ELEMENT_GRASSBUSH_DARK_57) TEXTURE(ELEMENT_GRASSBUSH_DARK_58) TEXTURE(ELEMENT_GRASSBUSH_DARK_59)\
  TEXTURE(ELEMENT_GRASSBUSH_REG_0) TEXTURE(ELEMENT_GRASSBUSH_REG_1) TEXTURE(ELEMENT_GRASSBUSH_REG_2) TEXTURE(ELEMENT_GRASSBUSH_REG_3) TEXTURE(ELEMENT_GRASSBUSH_REG_4) TEXTURE(ELEMENT_GRASSBUSH_REG_5) TEXTURE(ELEMENT_GRASSBUSH_REG_6) TEXTURE(ELEMENT_GRASSBUSH_REG_7) TEXTURE(ELEMENT_GRASSBUSH_REG_8) TEXTURE(ELEMENT_GRASSBUSH_REG_9)\
  TEXTURE(ELEMENT_GRASSBUSH_REG_10) TEXTURE(ELEMENT_GRASSBUSH_REG_11) TEXTURE(ELEMENT_GRASSBUSH_REG_12) TEXTURE(ELEMENT_GRASSBUSH_REG_13) TEXTURE(ELEMENT_GRASSBUSH_REG_14) TEXTURE(ELEMENT_GRASSBUSH_REG_15) TEXTURE(ELEMENT_GRASSBUSH_REG_16) TEXTURE(ELEMENT_GRASSBUSH_REG_17) TEXTURE(ELEMENT_GRASSBUSH_REG_18) TEXTURE(ELEMENT_GRASSBUSH_REG_19)\
  TEXTURE(ELEMENT_GRASSBUSH_REG_20) TEXTURE(ELEMENT_GRASSBUSH_REG_21) TEXTURE(ELEMENT_GRASSBUSH_REG_22) TEXTURE(ELEMENT_GRASSBUSH_REG_23) TEXTURE(ELEMENT_GRASSBUSH_REG_24) TEXTURE(ELEMENT_GRASSBUSH_REG_25) TEXTURE(ELEMENT_GRASSBUSH_REG_26) TEXTURE(ELEMENT_GRASSBUSH_REG_27) TEXTURE(ELEMENT_GRASSBUSH_REG_28) TEXTURE(ELEMENT_GRASSBUSH_REG_29)\
  TEXTURE(ELEMENT_GRASSBUSH_REG_30) TEXTURE(ELEMENT_GRASSBUSH_REG_31) TEXTURE(ELEMENT_GRASSBUSH_REG_32) TEXTURE(ELEMENT_GRASSBUSH_REG_33) TEXTURE(ELEMENT_GRASSBUSH_REG_34) TEXTURE(ELEMENT_GRASSBUSH_REG_35) TEXTURE(ELEMENT_GRASSBUSH_REG_36) TEXTURE(ELEMENT_GRASSBUSH_REG_37) TEXTURE(ELEMENT_GRASSBUSH_REG_38) TEXTURE(ELEMENT_GRASSBUSH_REG_39)\
  TEXTURE(ELEMENT_GRASSBUSH_REG_40) TEXTURE(ELEMENT_GRASSBUSH_REG_41) TEXTURE(ELEMENT_GRASSBUSH_REG_42) TEXTURE(ELEMENT_GRASSBUSH_REG_43) TEXTURE(ELEMENT_GRASSBUSH_REG_44) TEXTURE(ELEMENT_GRASSBUSH_REG_45) TEXTURE(ELEMENT_GRASSBUSH_REG_46) TEXTURE(ELEMENT_GRASSBUSH_REG_47) TEXTURE(ELEMENT_GRASSBUSH_REG_48) TEXTURE(ELEMENT_GRASSBUSH_REG_49)\
  TEXTURE(ELEMENT_GRASSBUSH_REG_50) TEXTURE(ELEMENT_GRASSBUSH_REG_51) TEXTURE(ELEMENT_GRASSBUSH_REG_52) TEXTURE(ELEMENT_GRASSBUSH_REG_53) TEXTURE(ELEMENT_GRASSBUSH_REG_54) TEXTURE(ELEMENT_GRASSBUSH_REG_55) TEXTURE(ELEMENT_GRASSBUSH_REG_56) TEXTURE(ELEMENT_GRASSBUSH_REG_57) TEXTURE(ELEMENT_GRASSBUSH_REG_58) TEXTURE(ELEMENT_GRASSBUSH_REG_59)\
  TEXTURE(ELEMENT_GRASSMARKS_0) TEXTURE(ELEMENT_GRASSMARKS_1) TEXTURE(ELEMENT_GRASSMARKS_2) TEXTURE(ELEMENT_GRASSMARKS_3) TEXTURE(ELEMENT_GRASSMARKS_4) TEXTURE(ELEMENT_GRASSMARKS_5) TEXTURE(ELEMENT_GRASSMARKS_6) TEXTURE(ELEMENT_GRASSMARKS_7) TEXTURE(ELEMENT_GRASSMARKS_8) TEXTURE(ELEMENT_GRASSMARKS_9)\
  TEXTURE(ELEMENT_GRASSMARKS_10) TEXTURE(ELEMENT_GRASSMARKS_11) TEXTURE(ELEMENT_GRASSMARKS_12) TEXTURE(ELEMENT_GRASSMARKS_13) TEXTURE(ELEMENT_GRASSMARKS_14) TEXTURE(ELEMENT_GRASSMARKS_15) TEXTURE(ELEMENT_GRASSMARKS_16) TEXTURE(ELEMENT_GRASSMARKS_17) TEXTURE(ELEMENT_GRASSMARKS_18) TEXTURE(ELEMENT_GRASSMARKS_19)\
  TEXTURE(ELEMENT_GRASSMARKS_20) TEXTURE(ELEMENT_GRASSMARKS_21) TEXTURE(ELEMENT_GRASSMARKS_22) TEXTURE(ELEMENT_GRASSMARKS_23) TEXTURE(ELEMENT_GRASSMARKS_24) TEXTURE(ELEMENT_GRASSMARKS_25) TEXTURE(ELEMENT_GRASSMARKS_26) TEXTURE(ELEMENT_GRASSMARKS_27) TEXTURE(ELEMENT_GRASSMARKS_28) TEXTURE(ELEMENT_GRASSMARKS_29)\
  TEXTURE(ELEMENT_GRASSMARKS_30) TEXTURE(ELEMENT_GRASSMARKS_31) TEXTURE(ELEMENT_GRASSMARKS_32) TEXTURE(ELEMENT_GRASSMARKS_33) TEXTURE(ELEMENT_GRASSMARKS_34) TEXTURE(ELEMENT_GRASSMARKS_35) TEXTURE(ELEMENT_GRASSMARKS_36) TEXTURE(ELEMENT_GRASSMARKS_37) TEXTURE(ELEMENT_GRASSMARKS_38) TEXTURE(ELEMENT_GRASSMARKS_39)\
  TEXTURE(ELEMENT_GRASSMARKS_40) TEXTURE(ELEMENT_GRASSMARKS_41) TEXTURE(ELEMENT_GRASSMARKS_42) TEXTURE(ELEMENT_GRASSMARKS_43) TEXTURE(ELEMENT_GRASSMARKS_44) TEXTURE(ELEMENT_GRASSMARKS_45) TEXTURE(ELEMENT_GRASSMARKS_46) TEXTURE(ELEMENT_GRASSMARKS_47) TEXTURE(ELEMENT_GRASSMARKS_48) TEXTURE(ELEMENT_GRASSMARKS_49)\
  TEXTURE(ELEMENT_GRASSMARKS_50) TEXTURE(ELEMENT_GRASSMARKS_51) TEXTURE(ELEMENT_GRASSMARKS_52) TEXTURE(ELEMENT_GRASSMARKS_53) TEXTURE(ELEMENT_GRASSMARKS_54) TEXTURE(ELEMENT_GRASSMARKS_55) TEXTURE(ELEMENT_GRASSMARKS_56) TEXTURE(ELEMENT_GRASSMARKS_57) TEXTURE(ELEMENT_GRASSMARKS_58) TEXTURE(ELEMENT_GRASSMARKS_59)\
  TEXTURE(ELEMENT_GRASSPATCH_0) TEXTURE(ELEMENT_GRASSPATCH_1) TEXTURE(ELEMENT_GRASSPATCH_2) TEXTURE(ELEMENT_GRASSPATCH_3) TEXTURE(ELEMENT_GRASSPATCH_4) TEXTURE(ELEMENT_GRASSPATCH_5) TEXTURE(ELEMENT_GRASSPATCH_6) TEXTURE(ELEMENT_GRASSPATCH_7) TEXTURE(ELEMENT_GRASSPATCH_8) TEXTURE(ELEMENT_GRASSPATCH_9)\
  TEXTURE(ELEMENT_GRASSPATCH_10) TEXTURE(ELEMENT_GRASSPATCH_11) TEXTURE(ELEMENT_GRASSPATCH_12) TEXTURE(ELEMENT_GRASSPATCH_13) TEXTURE(ELEMENT_GRASSPATCH_14) TEXTURE(ELEMENT_GRASSPATCH_15) TEXTURE(ELEMENT_GRASSPATCH_16) TEXTURE(ELEMENT_GRASSPATCH_17) TEXTURE(ELEMENT_GRASSPATCH_18) TEXTURE(ELEMENT_GRASSPATCH_19)\
  TEXTURE(ELEMENT_GRASSPATCH_20) TEXTURE(ELEMENT_GRASSPATCH_21) TEXTURE(ELEMENT_GRASSPATCH_22) TEXTURE(ELEMENT_GRASSPATCH_23) TEXTURE(ELEMENT_GRASSPATCH_24) TEXTURE(ELEMENT_GRASSPATCH_25) TEXTURE(ELEMENT_GRASSPATCH_26) TEXTURE(ELEMENT_GRASSPATCH_27) TEXTURE(ELEMENT_GRASSPATCH_28) TEXTURE(ELEMENT_GRASSPATCH_29)\
  TEXTURE(ELEMENT_GRASSPATCH_30) TEXTURE(ELEMENT_GRASSPATCH_31) TEXTURE(ELEMENT_GRASSPATCH_32) TEXTURE(ELEMENT_GRASSPATCH_33) TEXTURE(ELEMENT_GRASSPATCH_34) TEXTURE(ELEMENT_GRASSPATCH_35) TEXTURE(ELEMENT_GRASSPATCH_36) TEXTURE(ELEMENT_GRASSPATCH_37) TEXTURE(ELEMENT_GRASSPATCH_38) TEXTURE(ELEMENT_GRASSPATCH_39)\
  TEXTURE(ELEMENT_GRASSPATCH_40) TEXTURE(ELEMENT_GRASSPATCH_41) TEXTURE(ELEMENT_GRASSPATCH_42) TEXTURE(ELEMENT_GRASSPATCH_43) TEXTURE(ELEMENT_GRASSPATCH_44) TEXTURE(ELEMENT_GRASSPATCH_45) TEXTURE(ELEMENT_GRASSPATCH_46) TEXTURE(ELEMENT_GRASSPATCH_47) TEXTURE(ELEMENT_GRASSPATCH_48) TEXTURE(ELEMENT_GRASSPATCH_49)\
  TEXTURE(ELEMENT_GRASSPATCH_50) TEXTURE(ELEMENT_GRASSPATCH_51) TEXTURE(ELEMENT_GRASSPATCH_52) TEXTURE(ELEMENT_GRASSPATCH_53) TEXTURE(ELEMENT_GRASSPATCH_54) TEXTURE(ELEMENT_GRASSPATCH_55) TEXTURE(ELEMENT_GRASSPATCH_56) TEXTURE(ELEMENT_GRASSPATCH_57) TEXTURE(ELEMENT_GRASSPATCH_58) TEXTURE(ELEMENT_GRASSPATCH_59)\
  TEXTURE(ELEMENT_TALLGRASS_0) TEXTURE(ELEMENT_TALLGRASS_1) TEXTURE(ELEMENT_TALLGRASS_2) TEXTURE(ELEMENT_TALLGRASS_3) TEXTURE(ELEMENT_TALLGRASS_4) TEXTURE(ELEMENT_TALLGRASS_5) TEXTURE(ELEMENT_TALLGRASS_6) TEXTURE(ELEMENT_TALLGRASS_7) TEXTURE(ELEMENT_TALLGRASS_8) TEXTURE(ELEMENT_TALLGRASS_9)\
  TEXTURE(ELEMENT_TALLGRASS_10) TEXTURE(ELEMENT_TALLGRASS_11) TEXTURE(ELEMENT_TALLGRASS_12) TEXTURE(ELEMENT_TALLGRASS_13) TEXTURE(ELEMENT_TALLGRASS_14) TEXTURE(ELEMENT_TALLGRASS_15) TEXTURE(ELEMENT_TALLGRASS_16) TEXTURE(ELEMENT_TALLGRASS_17) TEXTURE(ELEMENT_TALLGRASS_18) TEXTURE(ELEMENT_TALLGRASS_19)\
  TEXTURE(ELEMENT_TALLGRASS_20) TEXTURE(ELEMENT_TALLGRASS_21) TEXTURE(ELEMENT_TALLGRASS_22) TEXTURE(ELEMENT_TALLGRASS_23) TEXTURE(ELEMENT_TALLGRASS_24) TEXTURE(ELEMENT_TALLGRASS_25) TEXTURE(ELEMENT_TALLGRASS_26) TEXTURE(ELEMENT_TALLGRASS_27) TEXTURE(ELEMENT_TALLGRASS_28) TEXTURE(ELEMENT_TALLGRASS_29)\
  TEXTURE(ELEMENT_TALLGRASS_30) TEXTURE(ELEMENT_TALLGRASS_31) TEXTURE(ELEMENT_TALLGRASS_32) TEXTURE(ELEMENT_TALLGRASS_33) TEXTURE(ELEMENT_TALLGRASS_34) TEXTURE(ELEMENT_TALLGRASS_35) TEXTURE(ELEMENT_TALLGRASS_36) TEXTURE(ELEMENT_TALLGRASS_37) TEXTURE(ELEMENT_TALLGRASS_38) TEXTURE(ELEMENT_TALLGRASS_39)\
  TEXTURE(ELEMENT_TALLGRASS_40) TEXTURE(ELEMENT_TALLGRASS_41) TEXTURE(ELEMENT_TALLGRASS_42) TEXTURE(ELEMENT_TALLGRASS_43) TEXTURE(ELEMENT_TALLGRASS_44) TEXTURE(ELEMENT_TALLGRASS_45) TEXTURE(ELEMENT_TALLGRASS_46) TEXTURE(ELEMENT_TALLGRASS_47) TEXTURE(ELEMENT_TALLGRASS_48) TEXTURE(ELEMENT_TALLGRASS_49)\
  TEXTURE(ELEMENT_TALLGRASS_50) TEXTURE(ELEMENT_TALLGRASS_51) TEXTURE(ELEMENT_TALLGRASS_52) TEXTURE(ELEMENT_TALLGRASS_53) TEXTURE(ELEMENT_TALLGRASS_54) TEXTURE(ELEMENT_TALLGRASS_55) TEXTURE(ELEMENT_TALLGRASS_56) TEXTURE(ELEMENT_TALLGRASS_57) TEXTURE(ELEMENT_TALLGRASS_58) TEXTURE(ELEMENT_TALLGRASS_59)\
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
const unsigned int get_tex_id_from_string(const char *tex_key);

void add_texture(void);
const gfx_texture get_texture(const unsigned int tex_id);
const gfx_texture get_texture_from_string(const char *tex_key);

void generate_textures(void);
void reset_textures(void);
void load_texture_tick(const unsigned int count);

