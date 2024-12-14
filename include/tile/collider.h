#pragma once

#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/array.h"

typedef struct area_collider {
  bool enabled;
  phy_rigid_body *body;
  phy_shape *shape;
} area_collider;

typedef struct area_colliders {
  bool enabled;
  utl_array *colliders;
} area_colliders;

const area_collider create_collider(phy_vector2 pos, phy_vector2 size);
void free_collider(area_collider *collider);
void free_colliders(utl_array *colliders);

void render_colliders(utl_array *tiles, int parallax_value);

