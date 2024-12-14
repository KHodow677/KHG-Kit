#pragma once

#include "khg_phy/body.h"
#include "khg_phy/shape.h"
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

