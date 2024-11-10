#pragma once

#include "khg_phy/constraints/constraint.h"

typedef struct {
  phy_vector2 anchor_a;
  phy_vector2 anchor_b;
  phy_vector2 xanchor_a;
  phy_vector2 xanchor_b;
  phy_vector2 normal;
  float bias;
  float mass;
  float impulse;
  float max_impulse;
  phy_vector2 anchor;
  float max_force;
  phy_vector2 controls[PHY_SPLINE_CONSTRAINT_MAX_CONTROL_POINTS];
  size_t num_controls;
} phy_spline_constraint;

typedef struct {
  phy_rigid_body *body;
  phy_vector2 anchor;
  float max_force;
} phy_spline_constraint_initializer;

static const phy_spline_constraint_initializer phy_spline_constraint_initializer_default = { NULL, { 0.0, 0.0 }, INFINITY };

phy_constraint *phy_spline_constraint_new(phy_spline_constraint_initializer init);

phy_rigid_body *phy_spline_constraint_get_body(const phy_constraint *cons);

void phy_spline_constraint_set_anchor(phy_constraint *cons, phy_vector2 anchor);
phy_vector2 phy_spline_constraint_get_anchor(const phy_constraint *cons);

void phy_spline_constraint_set_max_force(phy_constraint *cons, nv_float max_force);
nv_float phy_spline_constraint_get_max_force(const phy_constraint *cons);

int phy_spline_constraint_set_control_points(phy_constraint *cons, phy_vector2 *points, size_t num_points);
phy_vector2 *phy_spline_constraint_get_control_points(const phy_constraint *cons);
size_t phy_spline_constraint_get_number_of_control_points(const phy_constraint *cons);

void phy_spline_constraint_presolve(struct phy_space *space, phy_constraint *cons, nv_float dt, nv_float inv_dt);
void phy_spline_constraint_warmstart(struct phy_space *space, phy_constraint *cons);
void phy_spline_constraint_solve(phy_constraint *cons);

