#pragma once

#include "khg_phy/core/phy_vector.h"
#include <stddef.h>

typedef struct ovr_tile_element {
  unsigned int element_tex_id;
  phy_vector2 pos;
  bool flipped;
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

void set_ovr_tile_scale(const float tile_scale);
void set_ovr_tile_size(const unsigned int tile_tex_id);

phy_vector2 ovr_tile_pos_to_world_pos(const phy_vector2 coords);

phy_vector2 get_ovr_tile_rendering_pos(const phy_vector2 coords, const phy_vector2 offset, const float tex_height);
void render_ovr_tile_item(const ovr_tile tile, const unsigned int tex_id, const phy_vector2 coords, const phy_vector2 offset, const bool flipped);
void render_ovr_tile(const unsigned int tile_id, const phy_vector2 tile_pos);

void render_ovr_tile_ground(const unsigned int tile_id, const phy_vector2 tile_pos);
void render_ovr_tile_border(const unsigned int tile_id, const phy_vector2 tile_pos);
