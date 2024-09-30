#include "khg_phy/phy_types.h"

phy_space *physics_setup(phy_vect grav);
void physics_free(phy_space *sp);

phy_shape *physics_add_static_segment_shape(phy_space *space, phy_vect point_a, phy_vect point_b);
void physics_remove_static_segment_shape(phy_space *space, phy_shape *seg);

void physics_add_sensor_collision_handler(phy_space *space);

