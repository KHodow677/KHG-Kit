#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"

static gfx_texture NO_TEXTURE = { 0 };
static gfx_texture TEXTURE_LOOKUP[NUM_TEXTURES];
static texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

const gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

const bool check_texture_loaded(int tex_id) {
  return (TEXTURE_LOOKUP[tex_id].id != NO_TEXTURE.id);
}

const gfx_texture get_or_add_texture(int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return TEXTURE_LOOKUP[tex_id];
  }
  const texture_asset ta = TEXTURE_ASSET_REF[tex_id];
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
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_ROCK_0] = (texture_asset){ "main/environment/rocks/rock_00", "png", 302, 104, 0 };
  TEXTURE_ASSET_REF[MAIN_STATION_CART] = (texture_asset){ "main/station/cart", "png", 370, 280, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_0] = (texture_asset){ "player/idle/idle_00", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_1] = (texture_asset){ "player/idle/idle_01", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_2] = (texture_asset){ "player/idle/idle_02", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_3] = (texture_asset){ "player/idle/idle_03", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_4] = (texture_asset){ "player/idle/idle_04", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_5] = (texture_asset){ "player/idle/idle_05", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_6] = (texture_asset){ "player/idle/idle_06", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_7] = (texture_asset){ "player/idle/idle_07", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_8] = (texture_asset){ "player/idle/idle_08", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_9] = (texture_asset){ "player/idle/idle_09", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_10] = (texture_asset){ "player/idle/idle_10", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_11] = (texture_asset){ "player/idle/idle_11", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_12] = (texture_asset){ "player/idle/idle_12", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_13] = (texture_asset){ "player/idle/idle_13", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_14] = (texture_asset){ "player/idle/idle_14", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_15] = (texture_asset){ "player/idle/idle_15", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_16] = (texture_asset){ "player/idle/idle_16", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_17] = (texture_asset){ "player/idle/idle_17", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_18] = (texture_asset){ "player/idle/idle_18", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_19] = (texture_asset){ "player/idle/idle_19", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_20] = (texture_asset){ "player/idle/idle_20", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_21] = (texture_asset){ "player/idle/idle_21", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_22] = (texture_asset){ "player/idle/idle_22", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_23] = (texture_asset){ "player/idle/idle_23", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_24] = (texture_asset){ "player/idle/idle_24", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_25] = (texture_asset){ "player/idle/idle_25", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_26] = (texture_asset){ "player/idle/idle_26", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_27] = (texture_asset){ "player/idle/idle_27", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_28] = (texture_asset){ "player/idle/idle_28", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_29] = (texture_asset){ "player/idle/idle_29", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_30] = (texture_asset){ "player/idle/idle_30", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_31] = (texture_asset){ "player/idle/idle_31", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_32] = (texture_asset){ "player/idle/idle_32", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_33] = (texture_asset){ "player/idle/idle_33", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_34] = (texture_asset){ "player/idle/idle_34", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_35] = (texture_asset){ "player/idle/idle_35", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_36] = (texture_asset){ "player/idle/idle_36", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_37] = (texture_asset){ "player/idle/idle_37", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_38] = (texture_asset){ "player/idle/idle_38", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_39] = (texture_asset){ "player/idle/idle_39", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_40] = (texture_asset){ "player/idle/idle_40", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_41] = (texture_asset){ "player/idle/idle_41", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_42] = (texture_asset){ "player/idle/idle_42", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_43] = (texture_asset){ "player/idle/idle_43", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_44] = (texture_asset){ "player/idle/idle_44", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_45] = (texture_asset){ "player/idle/idle_45", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_46] = (texture_asset){ "player/idle/idle_46", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_47] = (texture_asset){ "player/idle/idle_47", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_48] = (texture_asset){ "player/idle/idle_48", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_49] = (texture_asset){ "player/idle/idle_49", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_50] = (texture_asset){ "player/idle/idle_50", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_51] = (texture_asset){ "player/idle/idle_51", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_52] = (texture_asset){ "player/idle/idle_52", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_53] = (texture_asset){ "player/idle/idle_53", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_54] = (texture_asset){ "player/idle/idle_54", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_55] = (texture_asset){ "player/idle/idle_55", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_56] = (texture_asset){ "player/idle/idle_56", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_57] = (texture_asset){ "player/idle/idle_57", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_58] = (texture_asset){ "player/idle/idle_58", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_59] = (texture_asset){ "player/idle/idle_59", "png", 166, 201, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_00] = (texture_asset){ "player/walk/walk_00", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_01] = (texture_asset){ "player/walk/walk_01", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_02] = (texture_asset){ "player/walk/walk_02", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_03] = (texture_asset){ "player/walk/walk_03", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_04] = (texture_asset){ "player/walk/walk_04", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_05] = (texture_asset){ "player/walk/walk_05", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_06] = (texture_asset){ "player/walk/walk_06", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_07] = (texture_asset){ "player/walk/walk_07", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_08] = (texture_asset){ "player/walk/walk_08", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_09] = (texture_asset){ "player/walk/walk_09", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_10] = (texture_asset){ "player/walk/walk_10", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_11] = (texture_asset){ "player/walk/walk_11", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_12] = (texture_asset){ "player/walk/walk_12", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_13] = (texture_asset){ "player/walk/walk_13", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_14] = (texture_asset){ "player/walk/walk_14", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_15] = (texture_asset){ "player/walk/walk_15", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_16] = (texture_asset){ "player/walk/walk_16", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_17] = (texture_asset){ "player/walk/walk_17", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_18] = (texture_asset){ "player/walk/walk_18", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_19] = (texture_asset){ "player/walk/walk_19", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_20] = (texture_asset){ "player/walk/walk_20", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_21] = (texture_asset){ "player/walk/walk_21", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_22] = (texture_asset){ "player/walk/walk_22", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[PLAYER_WALK_23] = (texture_asset){ "player/walk/walk_23", "png", 189, 195, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_0] = (texture_asset){ "animal/elk/idle/idle_00", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_1] = (texture_asset){ "animal/elk/idle/idle_01", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_2] = (texture_asset){ "animal/elk/idle/idle_02", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_3] = (texture_asset){ "animal/elk/idle/idle_03", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_4] = (texture_asset){ "animal/elk/idle/idle_04", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_5] = (texture_asset){ "animal/elk/idle/idle_05", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_6] = (texture_asset){ "animal/elk/idle/idle_06", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_7] = (texture_asset){ "animal/elk/idle/idle_07", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_8] = (texture_asset){ "animal/elk/idle/idle_08", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_9] = (texture_asset){ "animal/elk/idle/idle_09", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_10] = (texture_asset){ "animal/elk/idle/idle_10", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_11] = (texture_asset){ "animal/elk/idle/idle_11", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_12] = (texture_asset){ "animal/elk/idle/idle_12", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_13] = (texture_asset){ "animal/elk/idle/idle_13", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_14] = (texture_asset){ "animal/elk/idle/idle_14", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_15] = (texture_asset){ "animal/elk/idle/idle_15", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_16] = (texture_asset){ "animal/elk/idle/idle_16", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_17] = (texture_asset){ "animal/elk/idle/idle_17", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_18] = (texture_asset){ "animal/elk/idle/idle_18", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_19] = (texture_asset){ "animal/elk/idle/idle_19", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_20] = (texture_asset){ "animal/elk/idle/idle_20", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_21] = (texture_asset){ "animal/elk/idle/idle_21", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_22] = (texture_asset){ "animal/elk/idle/idle_22", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_23] = (texture_asset){ "animal/elk/idle/idle_23", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_24] = (texture_asset){ "animal/elk/idle/idle_24", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_25] = (texture_asset){ "animal/elk/idle/idle_25", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_26] = (texture_asset){ "animal/elk/idle/idle_26", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_27] = (texture_asset){ "animal/elk/idle/idle_27", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_28] = (texture_asset){ "animal/elk/idle/idle_28", "png", 203, 230, 0 };
  TEXTURE_ASSET_REF[ANIMAL_ELK_IDLE_29] = (texture_asset){ "animal/elk/idle/idle_29", "png", 203, 230, 0 };
  for (int i = 0; i < NUM_TEXTURES; i++) {
    get_or_add_texture(i);
  }
}

