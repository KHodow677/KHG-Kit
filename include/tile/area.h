#pragma once

#include "khg_phy/aabb.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/array.h"
#include "khg_utl/vector.h"
#include <stddef.h>

typedef struct area_tile {
  phy_vector2 pos;
  size_t tex_id;
} area_tile;

typedef struct area_collider {
  phy_aabb aabb;
} area_collider;

typedef struct area_object {
  phy_rigid_body *body;
  phy_shape *shape;
  size_t tex_id;
} area_object;

typedef struct area {
  utl_array *tiles;
  utl_array *colliders;
  utl_vector *objects;
} area;

void create_area(area *a, size_t num_tiles, size_t num_colliders);
void free_area(area *a);
