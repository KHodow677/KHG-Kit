#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"
#include "khg_utl/algorithm.h"
#include <string.h>

static gfx_texture NO_TEXTURE = { 0 };
static gfx_texture TEXTURE_LOOKUP[NUM_TEXTURES];
static texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

static int compare_texture_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const gfx_texture generate_texture(char *filepath, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(filepath);
  tex.width = width;
  tex.height = height;
  tex.angle = 0;
  return tex;
}

const bool check_texture_loaded(int tex_id) {
  return (TEXTURE_LOOKUP[tex_id].id != NO_TEXTURE.id);
}

const int get_tex_id_from_string(const char *tex_key) {
  return (int)utl_algorithm_find_at(TEXTURE_STRINGS, TEXTURE_STRINGS_SIZE, sizeof(char *), tex_key, compare_texture_strings);
}

const gfx_texture get_or_add_texture(int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return TEXTURE_LOOKUP[tex_id];
  }
  const texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  TEXTURE_LOOKUP[tex_id] = generate_texture(ta.tex_filepath, ta.tex_width, ta.tex_height);
  return TEXTURE_LOOKUP[tex_id];
}

const gfx_texture get_or_add_texture_from_string(const char *tex_key) {
  const int tex_id = get_tex_id_from_string(tex_key);
  return get_or_add_texture(tex_id);
}

void generate_textures() {
  TEXTURE_ASSET_REF[SQUARE] = (texture_asset){ "res/assets/textures/square.png", 512, 512 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_GROUND] = (texture_asset){ "res/assets/textures/main/environment/ground_layer.png", 1282, 722 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_0] = (texture_asset){ "res/assets/textures/main/environment/tree_layer_00.png", 1282, 722 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_TREE_1] = (texture_asset){ "res/assets/textures/main/environment/tree_layer_01.png", 1282, 722 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_0] = (texture_asset){ "res/assets/textures/main/environment/mountain_layer_00.png", 1282, 722 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_MOUNTAIN_1] = (texture_asset){ "res/assets/textures/main/environment/mountain_layer_01.png", 1282, 722 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_BACKGROUND] = (texture_asset){ "res/assets/textures/main/environment/background_layer.png", 1282, 722 };
  TEXTURE_ASSET_REF[MAIN_ENVIRONMENT_ROCK_0] = (texture_asset){ "res/assets/textures/main/environment/rocks/rock_00.png", 302, 104 };
  TEXTURE_ASSET_REF[MAIN_STATION_CART] = (texture_asset){ "res/assets/textures/main/station/cart.png", 370, 280 };
  TEXTURE_ASSET_REF[MAIN_STATION_TWIN_HOUSE] = (texture_asset){ "res/assets/textures/main/station/twin_house.png", 460, 468 };
  TEXTURE_ASSET_REF[PLAYER_HEAD] = (texture_asset){ "res/assets/textures/player/player_head_eyes.png", 110, 105 };
  TEXTURE_ASSET_REF[PLAYER_BODY] = (texture_asset){ "res/assets/textures/player/player_body.png", 60, 49 };
  TEXTURE_ASSET_REF[PLAYER_ARM_L] = (texture_asset){ "res/assets/textures/player/player_arm_l.png", 40, 39 };
  TEXTURE_ASSET_REF[PLAYER_ARM_R] = (texture_asset){ "res/assets/textures/player/player_arm_r.png", 43, 38 };
  TEXTURE_ASSET_REF[PLAYER_LEG_L] = (texture_asset){ "res/assets/textures/player/player_leg_l.png", 23, 30 };
  TEXTURE_ASSET_REF[PLAYER_LEG_R] = (texture_asset){ "res/assets/textures/player/player_leg_r.png", 23, 30 };
  TEXTURE_ASSET_REF[PLAYER_LEG_R] = (texture_asset){ "res/assets/textures/player/player_leg_r.png", 23, 30 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_0] = (texture_asset){ "res/assets/textures/tiles/dungeon/0.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_1] = (texture_asset){ "res/assets/textures/tiles/dungeon/1.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_2] = (texture_asset){ "res/assets/textures/tiles/dungeon/2.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_3] = (texture_asset){ "res/assets/textures/tiles/dungeon/3.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_4] = (texture_asset){ "res/assets/textures/tiles/dungeon/4.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_5] = (texture_asset){ "res/assets/textures/tiles/dungeon/5.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_6] = (texture_asset){ "res/assets/textures/tiles/dungeon/6.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_7] = (texture_asset){ "res/assets/textures/tiles/dungeon/7.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_8] = (texture_asset){ "res/assets/textures/tiles/dungeon/8.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_9] = (texture_asset){ "res/assets/textures/tiles/dungeon/9.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_10] = (texture_asset){ "res/assets/textures/tiles/dungeon/10.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_11] = (texture_asset){ "res/assets/textures/tiles/dungeon/11.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_12] = (texture_asset){ "res/assets/textures/tiles/dungeon/12.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_13] = (texture_asset){ "res/assets/textures/tiles/dungeon/13.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_14] = (texture_asset){ "res/assets/textures/tiles/dungeon/14.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_15] = (texture_asset){ "res/assets/textures/tiles/dungeon/15.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_16] = (texture_asset){ "res/assets/textures/tiles/dungeon/16.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_17] = (texture_asset){ "res/assets/textures/tiles/dungeon/17.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_18] = (texture_asset){ "res/assets/textures/tiles/dungeon/18.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_19] = (texture_asset){ "res/assets/textures/tiles/dungeon/19.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_20] = (texture_asset){ "res/assets/textures/tiles/dungeon/20.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_21] = (texture_asset){ "res/assets/textures/tiles/dungeon/21.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_22] = (texture_asset){ "res/assets/textures/tiles/dungeon/22.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_23] = (texture_asset){ "res/assets/textures/tiles/dungeon/23.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_24] = (texture_asset){ "res/assets/textures/tiles/dungeon/24.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_25] = (texture_asset){ "res/assets/textures/tiles/dungeon/25.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_26] = (texture_asset){ "res/assets/textures/tiles/dungeon/26.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_27] = (texture_asset){ "res/assets/textures/tiles/dungeon/27.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_28] = (texture_asset){ "res/assets/textures/tiles/dungeon/28.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_29] = (texture_asset){ "res/assets/textures/tiles/dungeon/29.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_30] = (texture_asset){ "res/assets/textures/tiles/dungeon/30.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_31] = (texture_asset){ "res/assets/textures/tiles/dungeon/31.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_32] = (texture_asset){ "res/assets/textures/tiles/dungeon/32.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_33] = (texture_asset){ "res/assets/textures/tiles/dungeon/33.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_34] = (texture_asset){ "res/assets/textures/tiles/dungeon/34.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_35] = (texture_asset){ "res/assets/textures/tiles/dungeon/35.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_36] = (texture_asset){ "res/assets/textures/tiles/dungeon/36.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_37] = (texture_asset){ "res/assets/textures/tiles/dungeon/37.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_38] = (texture_asset){ "res/assets/textures/tiles/dungeon/38.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_39] = (texture_asset){ "res/assets/textures/tiles/dungeon/39.png", 256, 256 };
  for (int i = 0; i < NUM_TEXTURES; i++) {
    get_or_add_texture(i);
  }
}

void reset_textures() {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    TEXTURE_LOOKUP[i].id = NO_TEXTURE.id;
  }
}

