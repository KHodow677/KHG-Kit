#pragma once

#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include "khg_utl/array.h"
#include "khg_utl/vector.h"
#include <stddef.h>

typedef struct area_collider {
  const phy_rigid_body *body;
  const phy_shape *shape;
} area_collider;

typedef struct area_object {
  const phy_rigid_body *body;
  const phy_shape *shape;
  size_t tex_id;
} area_object;

typedef struct area {
  bool enabled;
  utl_array *tiles;
  utl_array *colliders;
  utl_vector *objects;
  int tiles_layer;
  int objects_layer;
} area;

void create_area(area *a, size_t num_tiles, size_t num_colliders, int tiles_layer, int objects_layer);
void free_area(area *a);

