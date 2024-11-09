#include "physics/physics.h"
#include "khg_phy/body.h"
#include "khg_phy/vector.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"

nvSpace *SPACE = NULL;

void physics_setup(const nvVector2 grav) {
  SPACE = nvSpace_new();
  nvSpace_set_gravity(SPACE, grav);
  nvSpaceSettings *settings = nvSpace_get_settings(SPACE);
  settings->penetration_slop = 0.0f;
}

void physics_cleanup() {
  nvSpace_free(SPACE);
}

void physics_add_static_segment_shape(segment *seg, const nvVector2 point_a, const nvVector2 point_b) {
  nvRigidBodyInitializer seg_body_init = nvRigidBodyInitializer_default;
  seg_body_init.type = nvRigidBodyType_STATIC;
  seg_body_init.position = NV_VECTOR2((point_a.x + point_b.x) / 2.0f, (point_a.y + point_b.y) / 2.0f);
  seg_body_init.material = nvMaterial_CONCRETE;
  seg->seg_body = nvRigidBody_new(seg_body_init);
  seg->seg_shape = nvRectShape_new(fabsf(point_a.x - point_b.x), fabsf(point_a.y - point_b.y), nvVector2_zero);
  nvRigidBody_add_shape(seg->seg_body, seg->seg_shape);
  nvSpace_add_rigidbody(SPACE, seg->seg_body);
}

void physics_remove_static_segment_shape(segment *seg) {
  nvSpace_remove_rigidbody(SPACE, seg->seg_body);
  nvRigidBody_remove_shape(seg->seg_body, seg->seg_shape);
  nvRigidBody_free(seg->seg_body);
  nvShape_free(seg->seg_shape);
}

