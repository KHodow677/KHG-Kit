#pragma once

#include "khg_phy/shape.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include <stdbool.h>

typedef struct phy_contact_solver_info {
  float normal_impulse;
  float tangent_impulse;
  float mass_normal;
  float mass_tangent;
  float velocity_bias;
  float position_bias;
  float friction;
} phy_contact_solver_info;

typedef struct phy_contact {
  phy_vector2 anchor_a;
  phy_vector2 anchor_b;
  float separation;
  unsigned long id;
  bool is_persisted;
  bool remove_invoked;
  phy_contact_solver_info solver_info;
} phy_contact;

typedef struct phy_persistent_contact_pair {
  phy_vector2 normal;
  phy_contact contacts[2];
  unsigned int contact_count;
  phy_shape *shape_a;
  phy_shape *shape_b;
  phy_rigid_body *body_a;
  phy_rigid_body *body_b;
} phy_persistent_contact_pair;

typedef struct phy_contact_event {
  phy_rigid_body *body_a;
  phy_rigid_body *body_b;
  phy_shape *shape_a;
  phy_shape *shape_b;
  phy_vector2 normal;
  float penetration;
  phy_vector2 position;
  phy_vector2 normal_impulse;
  phy_vector2 friction_impulse;
  unsigned long id;
} phy_contact_event;

typedef void (*phy_contact_listener_callback)(struct phy_space *space, phy_contact_event event, void *user_arg);

typedef struct phy_contact_listener {
  phy_contact_listener_callback on_contact_added;
  phy_contact_listener_callback on_contact_persisted;
  phy_contact_listener_callback on_contact_removed;
} phy_contact_listener;

static const phy_contact_solver_info phy_contact_solver_info_zero = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

static inline unsigned long phy_persistent_contact_pair_key(phy_shape *a, phy_shape *b) {
  unsigned int fpa = phy_u32_hash(a->id);
  unsigned int fpb = phy_u32_hash(b->id);
  return phy_u32_pair(fpa, fpb);
}

bool phy_persistent_contact_pair_penetrating(phy_persistent_contact_pair *pcp);
unsigned long phy_persistent_contact_pair_hash(void *item);
void phy_persistent_contact_pair_remove(struct phy_space *space, phy_persistent_contact_pair *pcp);

