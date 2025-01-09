#pragma once

#include "khg_phy/core/phy_vector.h"
#include "khg_utl/array.h"
#include <stddef.h>

typedef struct area_tile {
  phy_vector2 pos;
  unsigned int tex_id;
} area_tile;

typedef struct area_tiles {
  bool enabled;
  int tiles_layer;
  utl_array *tiles;
} area_tiles;

void render_tiles(utl_array *tiles, int parallax_value);

