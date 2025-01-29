#pragma once

#include "khg_phy/core/phy_vector.h"
#include "khg_utl/array.h"
#include "util/frame.h"
#include <stddef.h>

typedef struct ovr_tile_info {
  unsigned int tile_id;
  phy_vector2 pos;
} ovr_tile_info;

typedef struct ovr_tile_element {
  unsigned int element_tex_id;
  phy_vector2 pos;
  float angle;
  float target_angle;
  float current_angle;
  frame_info frame;
  bool flipped;
  bool stable;
  ovr_tile_info *parent_tile;
} ovr_tile_element;

typedef struct ovr_tile {
  unsigned int tile_id;
  unsigned int ground_tex_id;
  unsigned int border_tex_id;
  utl_array *elements;
  unsigned int num_elements;
} ovr_tile;

extern utl_vector *OVR_TILE_OPTIONS;

void set_ovr_tile_scale(const float tile_scale);
const float get_ovr_tile_size(void);

phy_vector2 ovr_tile_pos_to_world_pos(const phy_vector2 coords);
phy_vector2 ovr_tile_rendering_pos(const phy_vector2 coords, const phy_vector2 offset, const float tex_height, const float tex_angle);

void render_ovr_tile_item(const unsigned int tex_id, const phy_vector2 coords, const phy_vector2 offset);
void render_ovr_tile_element_item(const ovr_tile_element *element);
void update_ovr_tile_element_item(ovr_tile_element *element, const float dt);

void add_ovr_tile_elements(ovr_tile_info *parent_tile); 
void remove_ovr_tile_elements(ovr_tile_info *parent_tile);

void render_ovr_tile(const ovr_tile_info *tile, unsigned int *layer, const float dt);

