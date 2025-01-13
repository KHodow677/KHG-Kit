#pragma once

#include "khg_phy/core/phy_vector.h"
#include "khg_utl/vector.h"
#include <stddef.h>

typedef struct ovr_tile_element {
  bool flipped;
  phy_vector2 pos;
  unsigned int element_tex_id;
} ovr_tile_element;

typedef struct ovr_tile {
  unsigned int id;
  unsigned int ground_tex_id;
  unsigned int border_tex_id;
  ovr_tile_element elements[128];
  unsigned int num_elements;
} ovr_tile;

typedef struct ovr_tile_info {
  unsigned int id;
  phy_vector2 pos;
} ovr_tile_info;

typedef struct ovr_map {
  bool enabled;
  float tile_scale;
  utl_vector *tiles;
} ovr_map;

void set_ovr_map_tile_scale(ovr_map *map, const float tile_scale);
phy_vector2 get_ovr_map_pos(ovr_map *map, const phy_vector2 map_pos, const unsigned int tile_size, const phy_vector2 coords, const phy_vector2 offset, const float tex_height);

void render_item(ovr_map *map, const ovr_tile tile, const unsigned int tex_id, const unsigned int tile_size, const phy_vector2 map_pos, const phy_vector2 coords, const phy_vector2 perc_offset, const bool flipped);
void render_ovr_map(ovr_map *map, const phy_vector2 map_pos);

