/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/constraints/constraint.h"
#include "khg_phy/space.h"
#include "khg_phy/constraints/distance_constraint.h"
#include "khg_phy/constraints/hinge_constraint.h"
#include "khg_phy/constraints/spline_constraint.h"


/**
 * @file constraints/constraint.c
 * 
 * @brief Base constraint definition.
 */


void phy_constraint_free(phy_constraint *cons) {
    if (!cons) return;

    free(cons->def);
    free(cons);
}

void phy_constraint_presolve(
    phy_space *space,
    phy_constraint *cons,
    float dt,
    float inv_dt
) {
    switch (cons->type) {
        case PHY_CONSTRAINT_TYPE_DISTANCE:
            phy_distance_constraint_presolve(space, cons, dt, inv_dt);
            break;

        case PHY_CONSTRAINT_TYPE_HINGE:
            phy_hinge_constraint_presolve(space, cons, dt, inv_dt);
            break;

        case PHY_CONSTRAINT_TYPE_SPLINE:
            phy_spline_constraint_presolve(space, cons, dt, inv_dt);
            break;
    }
}

void phy_constraint_warmstart(phy_space *space, phy_constraint *cons) {
    switch (cons->type) {
        case PHY_CONSTRAINT_TYPE_DISTANCE:
            phy_distance_constraint_warmstart(space, cons);
            break;

        case PHY_CONSTRAINT_TYPE_HINGE:
            phy_hinge_constraint_warmstart(space, cons);
            break;

        case PHY_CONSTRAINT_TYPE_SPLINE:
            phy_spline_constraint_warmstart(space, cons);
            break;
    }
}

void phy_constraint_solve(phy_constraint *cons, float inv_dt) {
    switch (cons->type) {
        case PHY_CONSTRAINT_TYPE_DISTANCE:
            phy_distance_constraint_solve(cons);
            break;

        case PHY_CONSTRAINT_TYPE_HINGE:
            phy_hinge_constraint_solve(cons, inv_dt);
            break;

        case PHY_CONSTRAINT_TYPE_SPLINE:
            phy_spline_constraint_solve(cons);
            break;
    }
}
