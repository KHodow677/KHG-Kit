#include "physics/physics.h"
#include "thread/thread_manager.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/threaded_space.h"

phy_space *SPACE = NULL;

void physics_setup(const phy_vect grav) {
  SPACE = phy_threaded_space_new();
  phy_threaded_space_set_threads(SPACE, THREAD_COUNT - 1);
  phy_space_set_gravity(SPACE, grav);
  phy_space_set_collision_bias(SPACE, 0.0f);
}

void physics_cleanup() {
  phy_threaded_space_free(SPACE);
}

const phy_shape *physics_add_static_segment_shape(phy_space *space, const phy_vect point_a, const phy_vect point_b) {
  phy_shape *seg = phy_segment_shape_new(phy_space_get_static_body(space), point_a, point_b, 0);
  phy_space_add_shape(space, seg);
	return seg;
}

void physics_remove_static_segment_shape(phy_space *space, phy_shape *seg) {
  phy_space_remove_shape(space, seg);
  phy_shape_free(seg);
}

