#include "area/ovr_tile.h"
#include "khg_phy/core/phy_vector.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"
#include "scene/builders/map_builder.h"
#include "scene/builders/tile_builder.h"

void build_ovr_map(const phy_vector2 pos, const float scale) {
  set_ovr_tile_scale(0.25f);
  set_ovr_tile_size(GROUND_GRASS);
  build_ovr_tile(PLAINS_CLEARING_0, phy_vector2_new(0.0f, 0.0f));
  build_ovr_tile(PLAINS_CLEARING_1, phy_vector2_new(1.0f, 1.0f));
  build_ovr_tile(PLAINS_CLEARING_2, phy_vector2_new(1.0f, -1.0f));
  build_ovr_tile(PLAINS_CLEARING_3, phy_vector2_new(-1.0f, -1.0f));
  build_ovr_tile(PLAINS_CLEARING_4, phy_vector2_new(-1.0f, 1.0f));
  build_ovr_tile(PLAINS_CLEARING_5, phy_vector2_new(2.0f, 0.0f));
  build_ovr_tile(PLAINS_CLEARING_6, phy_vector2_new(0.0f, 2.0f));
  build_ovr_tile(PLAINS_CLEARING_7, phy_vector2_new(-2.0f, 0.0f));
}
