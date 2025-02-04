#define NAMESPACE_KIN_IMPL

#include "khg_kin/namespace.h"
#include "khg_kin/body.h"
#include "khg_kin/collision.h"
#include "khg_kin/engine.h"
#include "khg_kin/shape.h"
#include "khg_kin/vec.h"

kin_namespace NAMESPACE_KIN_INTERNAL = {
  .body_create = body_create,
  .body_deallocate = body_deallocate,
  .body_step = body_step,
  .body_reset = body_reset,
  .body_force_set = body_force_set,
  .body_torque_set = body_torque_set,
  .body_force_add = body_force_add,
  .body_torque_add = body_torque_add,
  .body_shape_add = body_shape_add,
  .body_shape_remove = body_shape_remove,
  .body_print = body_print,
  .world_to_local = world_to_local,
  .local_to_world = local_to_world,
  .collide_bb = collide_bb,
  .collide_shapes = collide_shapes,
  .engine_create = engine_create,
  .engine_deallocate = engine_deallocate,
  .engine_step = engine_step,
  .engine_body_add = engine_body_add,
  .engine_body_remove = engine_body_remove,
  .engine_print = engine_print,
  .shape_circle_create = shape_circle_create,
  .shape_polygon_create = shape_polygon_create,
  .shape_segment_create = shape_segment_create,
  .shape_deallocate = shape_deallocate,
  .shape_circle_pose_update = shape_circle_pose_update,
  .shape_circle_bb_update = shape_circle_bb_update,
  .shape_polygon_pose_update = shape_polygon_pose_update,
  .shape_polygon_bb_update = shape_polygon_bb_update,
  .shape_segment_pose_update = shape_segment_pose_update,
  .shape_segment_bb_update = shape_segment_bb_update,
  .shape_print = shape_print,
  .vec_create = vec_create, 
  .vec_copy = vec_copy, 
  .vec_sq_length = vec_sq_length, 
  .vec_length = vec_length, 
  .vec_perp = vec_perp, 
  .vec_add = vec_add, 
  .vec_sub = vec_sub, 
  .vec_dot = vec_dot, 
  .vec_cross = vec_cross, 
  .vec_scale = vec_scale, 
  .vec_dist = vec_dist, 
  .vec_rot = vec_rot, 
};

kin_namespace *NAMESPACE_KIN(void) {
  return &NAMESPACE_KIN_INTERNAL;
}

