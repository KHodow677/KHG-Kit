#pragma once

#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/vector.h"

extern nvSpace *SPACE;

typedef struct segment {
  nvRigidBody *seg_body;
  nvShape *seg_shape;
} segment;

void physics_setup(const nvVector2 grav);
void physics_cleanup(void);

void physics_add_static_segment_shape(segment *seg, const nvVector2 point_a, const nvVector2 point_b);
void physics_remove_static_segment_shape(segment *seg);

