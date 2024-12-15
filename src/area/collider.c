#include "area/collider.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/material.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_utl/array.h"
#include "physics/physics.h"
#include <stdio.h>

const area_collider create_collider(phy_vector2 pos, phy_vector2 size) {
  printf("POS: %f, %f\n", pos.x, pos.y);
  printf("SIZE: %f, %f\n", size.x, size.y);
  phy_rigid_body *collider_body;
  phy_rigid_body_initializer collider_body_init = phy_rigid_body_initializer_default;
  collider_body_init.position = pos;
  collider_body_init.material = (phy_material){ .density=1.0, .restitution=0.0, .friction=0.5 };
  collider_body = phy_rigid_body_new(collider_body_init);
  phy_shape *ground_shape = phy_rect_shape_new(size.x, size.y, phy_vector2_zero);
  phy_rigid_body_add_shape(collider_body, ground_shape);
  phy_space_add_rigidbody(SPACE, collider_body);
  return (area_collider){ true, collider_body, ground_shape };
}

void free_collider(area_collider *collider) {
  phy_space_remove_rigidbody(SPACE, collider->body);
  phy_rigid_body_remove_shape(collider->body, collider->shape);
  phy_rigid_body_free(collider->body);
  phy_shape_free(collider->shape);
}

void free_colliders(utl_array *colliders) {
  for (area_collider *ac = utl_array_begin(colliders); ac != (area_collider *)utl_array_end(colliders); ac++) {
    free_collider(ac);
  }
}

