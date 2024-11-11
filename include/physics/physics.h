#pragma once

#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_vector.h"

extern phy_space *SPACE;

typedef struct segment {
  phy_rigid_body *seg_body;
  phy_shape *seg_shape;
} segment;

void physics_setup(const phy_vector2 grav);
void physics_cleanup(void);

void physics_add_static_segment_shape(segment *seg, const phy_vector2 point_a, const phy_vector2 point_b);
void physics_remove_static_segment_shape(segment *seg);

