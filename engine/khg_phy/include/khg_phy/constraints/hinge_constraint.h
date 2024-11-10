#pragma once

#include "khg_phy/constraints/constraint.h"
#include <stdbool.h>

typedef struct phy_hinge_constraint {
  phy_vector2 anchor_a;
  phy_vector2 anchor_b;
  phy_vector2 xanchor_a;
  phy_vector2 xanchor_b;
  float upper_impulse;
  float lower_impulse;
  float upper_bias;
  float lower_bias;
  float reference_angle;
  float axial_mass;
  phy_vector2 normal;
  float bias;
  float mass;
  float impulse;
  float max_impulse;
  phy_vector2 anchor;
  bool enable_limits;
  float upper_limit;
  float lower_limit;
  float angle;
  float max_force;
} phy_hinge_constraint;

typedef struct phy_hinge_constraint_initializer {
  phy_rigid_body *a;
  phy_rigid_body *b;
  phy_vector2 anchor;
  bool enable_limits;
  float upper_limit;
  float lower_limit;
  float max_force;
} phy_hinge_constraint_initializer;

static const phy_hinge_constraint_initializer phy_hinge_constraint_initializer_default = { NULL, NULL, { 0.0, 0.0 }, false, NV_PI * 0.5, -NV_PI * 0.5, INFINITY };

phy_constraint *phy_hinge_constraint_new(phy_hinge_constraint_initializer init);

phy_rigid_body *phy_hinge_constraint_get_body_a(const phy_constraint *cons);
phy_rigid_body *phy_hinge_constraint_get_body_b(const phy_constraint *cons);

void phy_hinge_constraint_set_anchor(phy_constraint *cons, phy_vector2 anchor);
phy_vector2 phy_hinge_constraint_get_anchor(const phy_constraint *cons);

void phy_hinge_constraint_set_limits(phy_constraint *cons, bool limits);
bool phy_hinge_constraint_get_limits(const phy_constraint *cons);

void phy_hinge_constraint_set_upper_limit(phy_constraint *cons, float upper_limit);
float phy_hinge_constraint_get_upper_limit(const phy_constraint *cons);
void phy_hinge_constraint_set_lower_limit(phy_constraint *cons, float lower_limit);
float phy_hinge_constraint_get_lower_limit(const phy_constraint *cons);

void phy_hinge_constraint_set_max_force(phy_constraint *cons, float max_force);
float phy_hinge_constraint_get_max_force(const phy_constraint *cons);

void phy_hinge_constraint_presolve(struct phy_space *space, phy_constraint *cons, float dt, float inv_dt);
void phy_hinge_constraint_warmstart(struct phy_space *space, phy_constraint *cons);
void phy_hinge_constraint_solve(phy_constraint *cons, nv_float inv_dt);

