#include "khg_kin/namespace.h"
#include "scene/builders/map_builder.h"
#include "scene/builders/light_builder.h"
#include "scene/builders/tile_builder.h"
#include "util/ovr_tile.h"

void build_ovr_map(const float scale) {
  set_ovr_tile_scale(scale);
  build_light(0.0f, 0.0f, 100.0f);
  build_random_tile((kin_vec){ 0, 0 });
  /*build_random_tile(phy_vector2_new(-1, -1));*/
  /*build_random_tile(phy_vector2_new(-1, 1));*/
  /*build_random_tile(phy_vector2_new(1, -1));*/
  /*build_random_tile(phy_vector2_new(1, 1));*/
  /*build_random_tile(phy_vector2_new(0, -2));*/
  /*build_random_tile(phy_vector2_new(0, 2));*/
  /*build_random_tile(phy_vector2_new(-2, -2));*/
  /*build_random_tile(phy_vector2_new(-2, 2));*/
  /*build_random_tile(phy_vector2_new(2, -2));*/
  /*build_random_tile(phy_vector2_new(2, 2));*/
  /*build_random_tile(phy_vector2_new(0, -4));*/
  /*build_random_tile(phy_vector2_new(0, 4));*/
  /*build_random_tile(phy_vector2_new(-1, -3));*/
  /*build_random_tile(phy_vector2_new(-1, 3));*/
  /*build_random_tile(phy_vector2_new(1, -3));*/
  /*build_random_tile(phy_vector2_new(1, 3));*/
  /*build_random_tile(phy_vector2_new(-2, 0));*/
  /*build_random_tile(phy_vector2_new(2, 0));*/
}

