#pragma once

#include "khg_phy/phy_types.h"

extern phy_space *SPACE;

void physics_setup(const phy_vect grav);
void physics_cleanup(void);

const phy_shape *physics_add_static_segment_shape(phy_space *space, const phy_vect point_a, const phy_vect point_b);
void physics_remove_static_segment_shape(phy_space *space, phy_shape *seg);

