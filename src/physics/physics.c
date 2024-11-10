#include "physics/physics.h"
#include "khg_phy/body.h"
#include "khg_phy/vector.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"

phy_space *SPACE = NULL;

void physics_setup(const phy_vector2 grav) {
  SPACE = phy_space_new();
  phy_space_set_gravity(SPACE, grav);
  phy_space_settings *settings = phy_space_get_settings(SPACE);
  settings->penetration_slop = 0.0f;
}

void physics_cleanup() {
  phy_space_free(SPACE);
}

void physics_add_static_segment_shape(segment *seg, const phy_vector2 point_a, const phy_vector2 point_b) {
  phy_rigid_body_initializer seg_body_init = phy_rigid_body_initializer_default;
  seg_body_init.type = PHY_RIGID_BODY_TYPE_STATIC;
  seg_body_init.position = phy_vector2_new((point_a.x + point_b.x) / 2.0f, (point_a.y + point_b.y) / 2.0f);
  seg_body_init.material = phy_material_concrete;
  seg->seg_body = phy_rigid_body_new(seg_body_init);
  seg->seg_shape = phy_rect_shape_new(fabsf(point_a.x - point_b.x), fabsf(point_a.y - point_b.y), phy_vector2_zero);
  phy_rigid_body_add_shape(seg->seg_body, seg->seg_shape);
  phy_space_add_rigidbody(SPACE, seg->seg_body);
}

void physics_remove_static_segment_shape(segment *seg) {
  phy_space_remove_rigidbody(SPACE, seg->seg_body);
  phy_rigid_body_remove_shape(seg->seg_body, seg->seg_shape);
  phy_rigid_body_free(seg->seg_body);
  phy_shape_free(seg->seg_shape);
}

