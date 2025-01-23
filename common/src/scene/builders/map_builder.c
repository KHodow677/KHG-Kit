#include "area/ovr_tile.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/random.h"
#include "khg_utl/time.h"
#include "resources/texture_loader.h"
#include "scene/builders/map_builder.h"
#include "scene/builders/tile_builder.h"

void build_ovr_map(const phy_vector2 pos, const float scale) {
  set_ovr_tile_scale(0.25f);
  set_ovr_tile_size(GROUND_GRASS);
  utl_random_seed((unsigned int) utl_time_current_time_in_seconds());
  build_random_tile(phy_vector2_new(0, 0));
  build_random_tile(phy_vector2_new(-1, -1));
  build_random_tile(phy_vector2_new(-1, 1));
  build_random_tile(phy_vector2_new(1, -1));
  build_random_tile(phy_vector2_new(1, 1));
  build_random_tile(phy_vector2_new(0, -2));
  build_random_tile(phy_vector2_new(0, 2));
  build_random_tile(phy_vector2_new(-2, -2));
  build_random_tile(phy_vector2_new(-2, 2));
  build_random_tile(phy_vector2_new(2, -2));
  build_random_tile(phy_vector2_new(2, 2));
  build_random_tile(phy_vector2_new(0, -4));
  build_random_tile(phy_vector2_new(0, 4));
  build_random_tile(phy_vector2_new(-1, -3));
  build_random_tile(phy_vector2_new(-1, 3));
  build_random_tile(phy_vector2_new(1, -3));
  build_random_tile(phy_vector2_new(1, 3));
  build_random_tile(phy_vector2_new(-2, 0));
  build_random_tile(phy_vector2_new(2, 0));
}
