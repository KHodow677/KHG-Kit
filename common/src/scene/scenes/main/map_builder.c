#include "area/ovr_tile.h"
#include "khg_phy/core/phy_vector.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"
#include "scene/scenes/main/tile_builder.h"
#include "scene/scenes/main/map_builder.h"

void build_ovr_map(const phy_vector2 pos, const float scale) {
  set_ovr_tile_scale(0.25f);
  set_ovr_tile_size(GROUND_GRASS);
  build_ovr_tile(PLAINS_CLEARING_0, phy_vector2_new(0.0f, 0.0f));
  build_ovr_tile(PLAINS_CLEARING_1, phy_vector2_new(1.0f, 1.0f));
}
