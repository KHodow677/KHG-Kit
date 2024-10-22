#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"

static gfx_texture NO_TEXTURE = { 0 };
static gfx_texture TEXTURE_LOOKUP[NUM_TEXTURES];
static texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

bool check_texture_loaded(int tex_id) {
  return (TEXTURE_LOOKUP[tex_id].id != NO_TEXTURE.id);
}

gfx_texture get_or_add_texture(int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return TEXTURE_LOOKUP[tex_id];
  }
  texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  TEXTURE_LOOKUP[tex_id] = generate_texture(ta.tex_file_name, ta.tex_file_type, ta.tex_width, ta.tex_height);
  return TEXTURE_LOOKUP[tex_id];
}

void generate_textures() {
  TEXTURE_ASSET_REF[SQUARE] = (texture_asset){ "square", "png", 512, 512, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_GROUND] = (texture_asset){ "main/environment/ground_layer", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_0] = (texture_asset){ "main/environment/tree_layer_00", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_1] = (texture_asset){ "main/environment/tree_layer_01", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_0] = (texture_asset){ "main/environment/mountain_layer_00", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_1] = (texture_asset){ "main/environment/mountain_layer_01", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_BACKGROUND] = (texture_asset){ "main/environment/background_layer", "png", 1282, 722, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_0] = (texture_asset){ "player/idle/idle_00", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_1] = (texture_asset){ "player/idle/idle_01", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_2] = (texture_asset){ "player/idle/idle_02", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_3] = (texture_asset){ "player/idle/idle_03", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_4] = (texture_asset){ "player/idle/idle_04", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_5] = (texture_asset){ "player/idle/idle_05", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_6] = (texture_asset){ "player/idle/idle_06", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_7] = (texture_asset){ "player/idle/idle_07", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_8] = (texture_asset){ "player/idle/idle_08", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_9] = (texture_asset){ "player/idle/idle_09", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_10] = (texture_asset){ "player/idle/idle_10", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_11] = (texture_asset){ "player/idle/idle_11", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_12] = (texture_asset){ "player/idle/idle_12", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_13] = (texture_asset){ "player/idle/idle_13", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_14] = (texture_asset){ "player/idle/idle_14", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_15] = (texture_asset){ "player/idle/idle_15", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_16] = (texture_asset){ "player/idle/idle_16", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_17] = (texture_asset){ "player/idle/idle_17", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_18] = (texture_asset){ "player/idle/idle_18", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_19] = (texture_asset){ "player/idle/idle_19", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_20] = (texture_asset){ "player/idle/idle_20", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_21] = (texture_asset){ "player/idle/idle_21", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_22] = (texture_asset){ "player/idle/idle_22", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_23] = (texture_asset){ "player/idle/idle_23", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_24] = (texture_asset){ "player/idle/idle_24", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_25] = (texture_asset){ "player/idle/idle_25", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_26] = (texture_asset){ "player/idle/idle_26", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_27] = (texture_asset){ "player/idle/idle_27", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_28] = (texture_asset){ "player/idle/idle_28", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_29] = (texture_asset){ "player/idle/idle_29", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_30] = (texture_asset){ "player/idle/idle_30", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_31] = (texture_asset){ "player/idle/idle_31", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_32] = (texture_asset){ "player/idle/idle_32", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_33] = (texture_asset){ "player/idle/idle_33", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_34] = (texture_asset){ "player/idle/idle_34", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_35] = (texture_asset){ "player/idle/idle_35", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_36] = (texture_asset){ "player/idle/idle_36", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_37] = (texture_asset){ "player/idle/idle_37", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_38] = (texture_asset){ "player/idle/idle_38", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_39] = (texture_asset){ "player/idle/idle_39", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_40] = (texture_asset){ "player/idle/idle_40", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_41] = (texture_asset){ "player/idle/idle_41", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_42] = (texture_asset){ "player/idle/idle_42", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_43] = (texture_asset){ "player/idle/idle_43", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_44] = (texture_asset){ "player/idle/idle_44", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_45] = (texture_asset){ "player/idle/idle_45", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_46] = (texture_asset){ "player/idle/idle_46", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_47] = (texture_asset){ "player/idle/idle_47", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_48] = (texture_asset){ "player/idle/idle_48", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_49] = (texture_asset){ "player/idle/idle_49", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_50] = (texture_asset){ "player/idle/idle_50", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_51] = (texture_asset){ "player/idle/idle_51", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_52] = (texture_asset){ "player/idle/idle_52", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_53] = (texture_asset){ "player/idle/idle_53", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_54] = (texture_asset){ "player/idle/idle_54", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_55] = (texture_asset){ "player/idle/idle_55", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_56] = (texture_asset){ "player/idle/idle_56", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_57] = (texture_asset){ "player/idle/idle_57", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_58] = (texture_asset){ "player/idle/idle_58", "png", 136, 165, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_59] = (texture_asset){ "player/idle/idle_59", "png", 136, 165, 0 };
  for (int i = 0; i < NUM_TEXTURES; i++) {
    get_or_add_texture(i);
  }
}

