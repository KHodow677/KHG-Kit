#pragma once

#include "khg_phy/core/phy_vector.h"
#include "tile/area.h"

typedef struct area_tile {
  phy_vector2 pos;
  size_t tex_id;
} area_tile;

void render_tiles(area *a, int parallax_value);

