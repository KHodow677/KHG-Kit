#include "generators/components/texture_generator.h"
#include "entity/map.h"
#include "game_manager.h"
#include "khg_gfx/texture.h"
#include "khg_utl/vector.h"

texture_asset TEXTURE_ASSET_REF[NUM_TEXTURES];

gfx_texture generate_texture(char *file_name, char *file_type, float width, float height) {
  gfx_texture tex = gfx_load_texture_asset(file_name, file_type);
  tex.width = width;
  tex.height = height;
  return tex;
}

bool check_texture_loaded(int tex_id) {
  gfx_texture *texture = utl_vector_at(TEXTURE_LOOKUP, tex_id);
  return (texture->id != NO_TEXTURE.id);
}

gfx_texture *get_or_add_texture(int tex_id) {
  if (check_texture_loaded(tex_id)) {
    return utl_vector_at(TEXTURE_LOOKUP, tex_id);
  }
  texture_asset ta = TEXTURE_ASSET_REF[tex_id];
  gfx_texture *texture = utl_vector_at(TEXTURE_LOOKUP, tex_id);
  *texture = generate_texture(ta.tex_file_name, ta.tex_file_type, ta.tex_width, ta.tex_height);
  return texture;
}

void add_new_texture(int tex_id, char *file_name, char *file_type, float width, float height) {
  gfx_texture *texture = utl_vector_at(TEXTURE_LOOKUP, tex_id);
  *texture = generate_texture(file_name, file_type, width, height);
}

void generate_textures() {
  TEXTURE_LOOKUP = utl_vector_create(sizeof(gfx_texture));
  for (int i = 0; i < NUM_TEXTURES; i++) {
    utl_vector_push_back(TEXTURE_LOOKUP, &NO_TEXTURE);
  }
  TEXTURE_ASSET_REF[SOIL_0] = (texture_asset){ "environment/Soil00", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SOIL_1] = (texture_asset){ "environment/Soil01", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SOIL_2] = (texture_asset){ "environment/Soil02", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SOIL_3] = (texture_asset){ "environment/Soil03", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[BUILDING_0] = (texture_asset){ "environment/Building00", "png", 136, 136, SEGMENT_TOP_LEFT, 0 };
  TEXTURE_ASSET_REF[BUILDING_1] = (texture_asset){ "environment/Building01", "png", 136, 136, SEGMENT_TOP, 0 };
  TEXTURE_ASSET_REF[BUILDING_2] = (texture_asset){ "environment/Building02", "png", 136, 136, SEGMENT_TOP_RIGHT, 0 };
  TEXTURE_ASSET_REF[BUILDING_3] = (texture_asset){ "environment/Building03", "png", 136, 136, SEGMENT_LEFT, 0 };
  TEXTURE_ASSET_REF[BUILDING_4] = (texture_asset){ "environment/Building04", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[BUILDING_5] = (texture_asset){ "environment/Building05", "png", 136, 136, SEGMENT_RIGHT, 0 };
  TEXTURE_ASSET_REF[BUILDING_6] = (texture_asset){ "environment/Building06", "png", 136, 136, SEGMENT_BOTTOM_LEFT, 0 };
  TEXTURE_ASSET_REF[BUILDING_7] = (texture_asset){ "environment/Building07", "png", 136, 136, SEGMENT_BOTTOM, 0 };
  TEXTURE_ASSET_REF[BUILDING_8] = (texture_asset){ "environment/Building08", "png", 136, 136, SEGMENT_BOTTOM_RIGHT, 0 };
  TEXTURE_ASSET_REF[BUILDING_9] = (texture_asset){ "environment/Building09", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[BUILDING_10] = (texture_asset){ "environment/Building10", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[BUILDING_11] = (texture_asset){ "environment/Building11", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[BUILDING_12] = (texture_asset){ "environment/Building12", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PATH_0] = (texture_asset){ "environment/Path00", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PATH_1] = (texture_asset){ "environment/Path01", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PATH_2] = (texture_asset){ "environment/Path02", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PATH_3] = (texture_asset){ "environment/Path03", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PATH_4] = (texture_asset){ "environment/Path04", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PATH_5] = (texture_asset){ "environment/Path05", "png", 136, 136, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[HEDGE_0] = (texture_asset){ "environment/Hedge00", "png", 136, 136, SEGMENT_TOP_LEFT, 20 };
  TEXTURE_ASSET_REF[HEDGE_1] = (texture_asset){ "environment/Hedge01", "png", 136, 136, SEGMENT_TOP, 20 };
  TEXTURE_ASSET_REF[HEDGE_2] = (texture_asset){ "environment/Hedge02", "png", 136, 136, SEGMENT_TOP_RIGHT, 20 };
  TEXTURE_ASSET_REF[HEDGE_3] = (texture_asset){ "environment/Hedge03", "png", 136, 136, SEGMENT_LEFT, 20 };
  TEXTURE_ASSET_REF[HEDGE_4] = (texture_asset){ "environment/Hedge04", "png", 136, 136, SEGMENT_NONE, 20 };
  TEXTURE_ASSET_REF[HEDGE_5] = (texture_asset){ "environment/Hedge05", "png", 136, 136, SEGMENT_RIGHT, 20 };
  TEXTURE_ASSET_REF[HEDGE_6] = (texture_asset){ "environment/Hedge06", "png", 136, 136, SEGMENT_BOTTOM_LEFT, 20 };
  TEXTURE_ASSET_REF[HEDGE_7] = (texture_asset){ "environment/Hedge07", "png", 136, 136, SEGMENT_BOTTOM, 20 };
  TEXTURE_ASSET_REF[HEDGE_8] = (texture_asset){ "environment/Hedge08", "png", 136, 136, SEGMENT_BOTTOM_RIGHT, 20 };
  TEXTURE_ASSET_REF[HEDGE_9] = (texture_asset){ "environment/Hedge09", "png", 136, 136, SEGMENT_NONE, 20 };
  TEXTURE_ASSET_REF[HEDGE_10] = (texture_asset){ "environment/Hedge10", "png", 136, 136, SEGMENT_NONE, 20 };
  TEXTURE_ASSET_REF[HEDGE_11] = (texture_asset){ "environment/Hedge11", "png", 136, 136, SEGMENT_NONE, 20 };
  TEXTURE_ASSET_REF[HEDGE_12] = (texture_asset){ "environment/Hedge12", "png", 136, 136, SEGMENT_NONE, 20 };
  TEXTURE_ASSET_REF[HANGAR_ICON_BIG] = (texture_asset){ "icons/Hangar-Icon-Big", "png", 128, 128, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[TANK_BODY_ICON_SMALL] = (texture_asset){ "icons/Tank-Body-Icon-Small", "png", 64, 64, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[TANK_TOP_ICON_SMALL] = (texture_asset){ "icons/Tank-Top-Icon-Small", "png", 64, 64, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[TANK_BODY] = (texture_asset){ "Tank-Body-Blue", "png", 145, 184, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[TANK_TOP] = (texture_asset){ "Tank-Top-Blue", "png", 104, 211, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[TURRET_BASE] = (texture_asset){ "Turret-Base", "png", 100, 100, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[TURRET_TOP] = (texture_asset){ "Turret-Top", "png", 89, 137, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[HANGAR] = (texture_asset){ "Hangar", "png", 276, 304, SEGMENT_BOTTOM_LEFT_RIGHT, 0 };
  TEXTURE_ASSET_REF[SPAWNER] = (texture_asset){ "Spawner", "png", 191, 214, SEGMENT_BOTTOM_LEFT_RIGHT, 0 };
  TEXTURE_ASSET_REF[DEPOT] = (texture_asset){ "Depot", "png", 248, 273, SEGMENT_BOTTOM_LEFT_RIGHT, 0 };
  TEXTURE_ASSET_REF[TANK_BODY_OUTLINE] = (texture_asset){ "Tank-Body-Blue-Outline", "png", 165, 204, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[TANK_TOP_OUTLINE] = (texture_asset){ "Tank-Top-Blue-Outline", "png", 124, 231, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[HANGAR_OUTLINE] = (texture_asset){ "Hangar-Outline", "png", 306, 334, SEGMENT_BOTTOM_LEFT_RIGHT, 0 };
  TEXTURE_ASSET_REF[COMMAND_POINT] = (texture_asset){ "targeting/Point", "png", 56, 56, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[COMMAND_LINE] = (texture_asset){ "targeting/Line", "png", 20, 1, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PARTICLE_2_0] = (texture_asset){ "particle/p2/00", "png", 100, 109, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PARTICLE_2_1] = (texture_asset){ "particle/p2/01", "png", 100, 109, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PARTICLE_2_2] = (texture_asset){ "particle/p2/02", "png", 100, 109, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PARTICLE_2_3] = (texture_asset){ "particle/p2/03", "png", 100, 109, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[PARTICLE_2_4] = (texture_asset){ "particle/p2/04", "png", 100, 109, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_0] = (texture_asset){ "creatures/slug1/00", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_1] = (texture_asset){ "creatures/slug1/01", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_2] = (texture_asset){ "creatures/slug1/02", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_3] = (texture_asset){ "creatures/slug1/03", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_4] = (texture_asset){ "creatures/slug1/04", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_5] = (texture_asset){ "creatures/slug1/05", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_6] = (texture_asset){ "creatures/slug1/06", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_7] = (texture_asset){ "creatures/slug1/07", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_8] = (texture_asset){ "creatures/slug1/08", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_9] = (texture_asset){ "creatures/slug1/09", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_10] = (texture_asset){ "creatures/slug1/10", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_11] = (texture_asset){ "creatures/slug1/11", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_12] = (texture_asset){ "creatures/slug1/12", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_13] = (texture_asset){ "creatures/slug1/13", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_14] = (texture_asset){ "creatures/slug1/14", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_15] = (texture_asset){ "creatures/slug1/15", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_16] = (texture_asset){ "creatures/slug1/16", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_17] = (texture_asset){ "creatures/slug1/17", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_18] = (texture_asset){ "creatures/slug1/18", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_19] = (texture_asset){ "creatures/slug1/19", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_20] = (texture_asset){ "creatures/slug1/20", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_21] = (texture_asset){ "creatures/slug1/21", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_22] = (texture_asset){ "creatures/slug1/22", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_23] = (texture_asset){ "creatures/slug1/23", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_24] = (texture_asset){ "creatures/slug1/24", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_25] = (texture_asset){ "creatures/slug1/25", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_26] = (texture_asset){ "creatures/slug1/26", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_27] = (texture_asset){ "creatures/slug1/27", "png", 180, 225, SEGMENT_NONE, 0 };
  TEXTURE_ASSET_REF[SLUG_1_28] = (texture_asset){ "creatures/slug1/28", "png", 180, 225, SEGMENT_NONE, 0 };
}

void free_textures() {
  utl_vector_deallocate(TEXTURE_LOOKUP);
}
