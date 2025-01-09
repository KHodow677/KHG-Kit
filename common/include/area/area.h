#pragma once

#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include <stddef.h>

typedef struct area_object {
  phy_rigid_body *body;
  phy_shape *shape;
  unsigned int tex_id;
} area_object;

