#pragma once

#include "khg_phy/core/phy_vector.h"
#include <stddef.h>

typedef struct ovr_tile_element {
  phy_vector2 pos;
  unsigned int element_tex_id;
} ovr_tile_element;

typedef struct ovr_tile {
  unsigned int id;
  unsigned int ground_tex_id;
  unsigned int border_tex_id;
  ovr_tile_element elements[128];
} ovr_tile;

typedef struct ovr_tile_info {
  unsigned int id;
  phy_vector2 pos;
} ovr_tile_info;

