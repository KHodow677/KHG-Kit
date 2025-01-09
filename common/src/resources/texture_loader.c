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

const unsigned int get_tex_id_from_string(const char *tex_key) {
  return utl_algorithm_find_at(TEXTURE_STRINGS, TEXTURE_STRINGS_SIZE, sizeof(char *), tex_key, compare_texture_strings);
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
  const unsigned int tex_id = get_tex_id_from_string(tex_key);
  return get_or_add_texture(tex_id);
}

void generate_textures() {
  TEXTURE_ASSET_REF[SQUARE] = (texture_asset){ "res/assets/textures/square.png", 512, 512 };
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
  TEXTURE_ASSET_REF[TILE_DUNGEON_40] = (texture_asset){ "res/assets/textures/tiles/dungeon/40.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_41] = (texture_asset){ "res/assets/textures/tiles/dungeon/41.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_42] = (texture_asset){ "res/assets/textures/tiles/dungeon/42.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_43] = (texture_asset){ "res/assets/textures/tiles/dungeon/43.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_44] = (texture_asset){ "res/assets/textures/tiles/dungeon/44.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_45] = (texture_asset){ "res/assets/textures/tiles/dungeon/45.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_46] = (texture_asset){ "res/assets/textures/tiles/dungeon/46.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_47] = (texture_asset){ "res/assets/textures/tiles/dungeon/47.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_48] = (texture_asset){ "res/assets/textures/tiles/dungeon/48.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_49] = (texture_asset){ "res/assets/textures/tiles/dungeon/49.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_50] = (texture_asset){ "res/assets/textures/tiles/dungeon/50.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_51] = (texture_asset){ "res/assets/textures/tiles/dungeon/51.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_52] = (texture_asset){ "res/assets/textures/tiles/dungeon/52.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_53] = (texture_asset){ "res/assets/textures/tiles/dungeon/53.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_54] = (texture_asset){ "res/assets/textures/tiles/dungeon/54.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_55] = (texture_asset){ "res/assets/textures/tiles/dungeon/55.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_56] = (texture_asset){ "res/assets/textures/tiles/dungeon/56.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_57] = (texture_asset){ "res/assets/textures/tiles/dungeon/57.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_58] = (texture_asset){ "res/assets/textures/tiles/dungeon/58.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_59] = (texture_asset){ "res/assets/textures/tiles/dungeon/59.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_60] = (texture_asset){ "res/assets/textures/tiles/dungeon/60.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_61] = (texture_asset){ "res/assets/textures/tiles/dungeon/61.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_62] = (texture_asset){ "res/assets/textures/tiles/dungeon/62.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_63] = (texture_asset){ "res/assets/textures/tiles/dungeon/63.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_64] = (texture_asset){ "res/assets/textures/tiles/dungeon/64.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_65] = (texture_asset){ "res/assets/textures/tiles/dungeon/65.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_66] = (texture_asset){ "res/assets/textures/tiles/dungeon/66.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_67] = (texture_asset){ "res/assets/textures/tiles/dungeon/67.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_68] = (texture_asset){ "res/assets/textures/tiles/dungeon/68.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_69] = (texture_asset){ "res/assets/textures/tiles/dungeon/69.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_70] = (texture_asset){ "res/assets/textures/tiles/dungeon/70.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_71] = (texture_asset){ "res/assets/textures/tiles/dungeon/71.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_72] = (texture_asset){ "res/assets/textures/tiles/dungeon/72.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_73] = (texture_asset){ "res/assets/textures/tiles/dungeon/73.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_74] = (texture_asset){ "res/assets/textures/tiles/dungeon/74.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_75] = (texture_asset){ "res/assets/textures/tiles/dungeon/75.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_76] = (texture_asset){ "res/assets/textures/tiles/dungeon/76.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_77] = (texture_asset){ "res/assets/textures/tiles/dungeon/77.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_78] = (texture_asset){ "res/assets/textures/tiles/dungeon/78.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_79] = (texture_asset){ "res/assets/textures/tiles/dungeon/79.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_80] = (texture_asset){ "res/assets/textures/tiles/dungeon/80.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_81] = (texture_asset){ "res/assets/textures/tiles/dungeon/81.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_82] = (texture_asset){ "res/assets/textures/tiles/dungeon/82.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_83] = (texture_asset){ "res/assets/textures/tiles/dungeon/83.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_84] = (texture_asset){ "res/assets/textures/tiles/dungeon/84.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_85] = (texture_asset){ "res/assets/textures/tiles/dungeon/85.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_86] = (texture_asset){ "res/assets/textures/tiles/dungeon/86.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_87] = (texture_asset){ "res/assets/textures/tiles/dungeon/87.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_88] = (texture_asset){ "res/assets/textures/tiles/dungeon/88.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_89] = (texture_asset){ "res/assets/textures/tiles/dungeon/89.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_90] = (texture_asset){ "res/assets/textures/tiles/dungeon/90.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_91] = (texture_asset){ "res/assets/textures/tiles/dungeon/91.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_92] = (texture_asset){ "res/assets/textures/tiles/dungeon/92.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_93] = (texture_asset){ "res/assets/textures/tiles/dungeon/93.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_94] = (texture_asset){ "res/assets/textures/tiles/dungeon/94.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_95] = (texture_asset){ "res/assets/textures/tiles/dungeon/95.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_96] = (texture_asset){ "res/assets/textures/tiles/dungeon/96.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_97] = (texture_asset){ "res/assets/textures/tiles/dungeon/97.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_98] = (texture_asset){ "res/assets/textures/tiles/dungeon/98.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_99] = (texture_asset){ "res/assets/textures/tiles/dungeon/99.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_100] = (texture_asset){ "res/assets/textures/tiles/dungeon/100.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_101] = (texture_asset){ "res/assets/textures/tiles/dungeon/101.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_102] = (texture_asset){ "res/assets/textures/tiles/dungeon/102.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_103] = (texture_asset){ "res/assets/textures/tiles/dungeon/103.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_104] = (texture_asset){ "res/assets/textures/tiles/dungeon/104.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_105] = (texture_asset){ "res/assets/textures/tiles/dungeon/105.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_106] = (texture_asset){ "res/assets/textures/tiles/dungeon/106.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_107] = (texture_asset){ "res/assets/textures/tiles/dungeon/107.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_108] = (texture_asset){ "res/assets/textures/tiles/dungeon/108.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_109] = (texture_asset){ "res/assets/textures/tiles/dungeon/109.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_110] = (texture_asset){ "res/assets/textures/tiles/dungeon/110.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_111] = (texture_asset){ "res/assets/textures/tiles/dungeon/111.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_112] = (texture_asset){ "res/assets/textures/tiles/dungeon/112.png", 256, 256 };
  TEXTURE_ASSET_REF[TILE_DUNGEON_113] = (texture_asset){ "res/assets/textures/tiles/dungeon/113.png", 256, 256 };
  for (int i = 0; i < NUM_TEXTURES; i++) {
    get_or_add_texture(i);
  }
}

void reset_textures() {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    TEXTURE_LOOKUP[i].id = NO_TEXTURE.id;
  }
}

