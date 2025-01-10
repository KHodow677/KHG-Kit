#pragma once

#include "khg_phy/core/phy_vector.h"
#include "khg_utl/array.h"
#include <stddef.h>

typedef struct ovr_tile_element {
  phy_vector2 pos;
  unsigned int element_tex_id;
} ovr_tile_element;

typedef struct ovr_tile {
  unsigned int id;
  phy_vector2 pos;
  unsigned int ground_tex_id;
  unsigned int border_tex_id;
  utl_array *elements;
} ovr_tile;

typedef struct ovr_map {
  bool enabled;
  utl_vector *tiles;
} ovr_map;

void render_tiles(utl_vector *tiles, int parallax_value);

