#pragma once

#include "khg_phy/constraints/constraint.h"
#include <math.h>
#include <stdbool.h>

typedef struct phy_distance_constraint {
  phy_vector2 xanchor_a;
  phy_vector2 xanchor_b;
  phy_vector2 normal;
  float bias;
  float mass;
  float impulse;
  float max_impulse;
  float bias_rate;
  float mass_coeff;
  float impulse_coeff;
  float length;
  phy_vector2 anchor_a;
  phy_vector2 anchor_b;
  float max_force;
  bool spring;
  float hertz;
  float damping;
} phy_distance_constraint;

typedef struct phy_distance_constraint_initializer {
  phy_rigid_body *a;
  phy_rigid_body *b;
  float length;
  phy_vector2 anchor_a;
  phy_vector2 anchor_b;
  float max_force;
  bool spring;
  float hertz;
  float damping;
} phy_distance_constraint_initializer;

static const phy_distance_constraint_initializer phy_distance_constraint_initializer_default = { NULL, NULL, 1.0, { 0.0, 0.0 }, { 0.0, 0.0 }, INFINITY, false, 3.0, 0.3 };

phy_constraint *phy_distance_constraint_new(phy_distance_constraint_initializer init);

phy_rigid_body *phy_distance_constraint_get_body_a(const phy_constraint *cons);
phy_rigid_body *phy_distance_constraint_get_body_b(const phy_constraint *cons);

void phy_distance_constraint_set_length(phy_constraint *cons, float length);
float phy_distance_constraint_get_length(const phy_constraint *cons);

void phy_distance_constraint_set_anchor_a(phy_constraint *cons, phy_vector2 anchor_a);
phy_vector2 phy_distance_constraint_get_anchor_a(const phy_constraint *cons);

void phy_distance_constraint_set_anchor_b(phy_constraint *cons, phy_vector2 anchor_b);
phy_vector2 phy_distance_constraint_get_anchor_b(const phy_constraint *cons);

void phy_distance_constraint_set_max_force(phy_constraint *cons, float max_force);
float phy_distance_constraint_get_max_force(const phy_constraint *cons);

void phy_distance_constraint_set_spring(phy_constraint *cons, bool spring);
bool phy_distance_constraint_get_spring(const phy_constraint *cons);

void phy_distance_constraint_set_hertz(phy_constraint *cons, float hertz);
float phy_distance_constraint_get_hertz(const phy_constraint *cons);

void phy_distance_constraint_set_damping(phy_constraint *cons, float damping);
float phy_distance_constraint_get_damping(const phy_constraint *cons);

void phy_distance_constraint_presolve(struct phy_space *space, phy_constraint *cons, float dt, float inv_dt);
void phy_distance_constraint_warmstart(struct phy_space *space, phy_constraint *cons);
void phy_distance_constraint_solve(phy_constraint *cons);

