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
  TEXTURE_ASSET_REF[PLAYER_IDLE_0] = (texture_asset){ "res/assets/textures/player/idle/player_idle_0.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_1] = (texture_asset){ "res/assets/textures/player/idle/player_idle_1.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_2] = (texture_asset){ "res/assets/textures/player/idle/player_idle_2.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_3] = (texture_asset){ "res/assets/textures/player/idle/player_idle_3.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_4] = (texture_asset){ "res/assets/textures/player/idle/player_idle_4.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_5] = (texture_asset){ "res/assets/textures/player/idle/player_idle_5.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_6] = (texture_asset){ "res/assets/textures/player/idle/player_idle_6.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_IDLE_7] = (texture_asset){ "res/assets/textures/player/idle/player_idle_7.png", 300, 256 };
  TEXTURE_ASSET_REF[PLAYER_HEAD] = (texture_asset){ "res/assets/textures/player/player_head_eyes.png", 110, 105 };
  TEXTURE_ASSET_REF[PLAYER_BODY] = (texture_asset){ "res/assets/textures/player/player_body.png", 60, 49 };
  TEXTURE_ASSET_REF[PLAYER_ARM_L] = (texture_asset){ "res/assets/textures/player/player_arm_l.png", 40, 39 };
  TEXTURE_ASSET_REF[PLAYER_ARM_R] = (texture_asset){ "res/assets/textures/player/player_arm_r.png", 43, 38 };
  TEXTURE_ASSET_REF[PLAYER_LEG_L] = (texture_asset){ "res/assets/textures/player/player_leg_l.png", 23, 30 };
  TEXTURE_ASSET_REF[PLAYER_LEG_R] = (texture_asset){ "res/assets/textures/player/player_leg_r.png", 23, 30 };
  for (int i = 0; i < NUM_TEXTURES; i++) {
    get_or_add_texture(i);
  }
}

