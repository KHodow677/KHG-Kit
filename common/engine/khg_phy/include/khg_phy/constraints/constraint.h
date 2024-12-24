#pragma once

#include "khg_phy/body.h"
#include <math.h>
#include <stdbool.h>

typedef enum {
  PHY_CONTACT_POSITION_CORRECTION_BAUMGARTE,
  PHY_CONTACT_POSITION_CORRECTION_NGS
} phy_contact_position_correction;

typedef enum {
  PHY_COEFFICIENT_MIX_AVG,
  PHY_COEFFICIENT_MIX_MUL,
  PHY_COEFFICIENT_MIX_SQRT,
  PHY_COEFFICIENT_MIX_MIN,
  PHY_COEFFICIENT_MIX_MAX
} phy_coefficient_mix;

static inline float phy_mix_coefficients(float a, float b, phy_coefficient_mix mix) {
  switch (mix) {
    case PHY_COEFFICIENT_MIX_AVG:
      return (a + b) * (float)0.5;
    case PHY_COEFFICIENT_MIX_MUL:
      return a * b;
    case PHY_COEFFICIENT_MIX_SQRT:
      return sqrtf(a * b);
    case PHY_COEFFICIENT_MIX_MIN:
      return fminf(a, b);
    case PHY_COEFFICIENT_MIX_MAX:
      return fmaxf(a, b);
    default:
      return 0.0;
  }
}

typedef enum {
  PHY_CONSTRAINT_TYPE_DISTANCE,
  PHY_CONSTRAINT_TYPE_HINGE,
  PHY_CONSTRAINT_TYPE_SPLINE
} phy_constraint_type;

typedef struct phy_constraint {
  phy_constraint_type type;
  void *def;
  phy_rigid_body *a;
  phy_rigid_body *b;
  bool ignore_collision;
} phy_constraint;

void phy_constraint_free(phy_constraint *cons);

void phy_constraint_presolve(struct phy_space *space, phy_constraint *cons, float dt, float inv_dt);
void phy_constraint_warmstart(struct phy_space *space, phy_constraint *cons);
void phy_constraint_solve(phy_constraint *cons, float inv_dt);

