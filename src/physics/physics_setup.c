#include "physics/physics_setup.h"
#include "game_manager.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/threaded_space.h"

phy_space *physics_setup(phy_vect grav) {
  phy_space *sp = phy_threaded_space_new();
  phy_threaded_space_set_threads(sp, THREAD_COUNT);
  phy_space_set_gravity(sp, grav);
  phy_space_set_collision_bias(sp, 0.0f);
  physics_add_sensor_collision_handler(sp);
  return sp;
}

void physics_free(phy_space *sp) {
  phy_threaded_space_free(sp);
}

phy_shape *physics_add_static_segment_shape(phy_space *space, phy_vect point_a, phy_vect point_b) {
  phy_shape *seg = phy_segment_shape_new(phy_space_get_static_body(space), point_a, point_b, 0);
  phy_space_add_shape(space, seg);
	return seg;
}

void physics_remove_static_segment_shape(phy_space *space, phy_shape *seg) {
  phy_space_remove_shape(space, seg);
  phy_shape_free(seg);
}

void physics_add_sensor_collision_handler(phy_space *space) {
  phy_collision_handler *collision_handler = phy_space_add_collision_handler(space, SENSOR_COLLISION_TYPE, NORMAL_COLLISION_TYPE);
  collision_handler->begin_func = targeter_sensor_enter;
  collision_handler->separate_func = targeter_sensor_exit;
}

