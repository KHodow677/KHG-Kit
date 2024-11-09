/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/internal.h"
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


void nvConstraint_free(nvConstraint *cons) {
    if (!cons) return;

    NV_FREE(cons->def);
    NV_FREE(cons);
}

void nvConstraint_presolve(
    nvSpace *space,
    nvConstraint *cons,
    nv_float dt,
    nv_float inv_dt
) {
    switch (cons->type) {
        case nvConstraintType_DISTANCE:
            nvDistanceConstraint_presolve(space, cons, dt, inv_dt);
            break;

        case nvConstraintType_HINGE:
            nvHingeConstraint_presolve(space, cons, dt, inv_dt);
            break;

        case nvConstraintType_SPLINE:
            nvSplineConstraint_presolve(space, cons, dt, inv_dt);
            break;
    }
}

void nvConstraint_warmstart(nvSpace *space, nvConstraint *cons) {
    switch (cons->type) {
        case nvConstraintType_DISTANCE:
            nvDistanceConstraint_warmstart(space, cons);
            break;

        case nvConstraintType_HINGE:
            nvHingeConstraint_warmstart(space, cons);
            break;

        case nvConstraintType_SPLINE:
            nvSplineConstraint_warmstart(space, cons);
            break;
    }
}

void nvConstraint_solve(nvConstraint *cons, nv_float inv_dt) {
    switch (cons->type) {
        case nvConstraintType_DISTANCE:
            nvDistanceConstraint_solve(cons);
            break;

        case nvConstraintType_HINGE:
            nvHingeConstraint_solve(cons, inv_dt);
            break;

        case nvConstraintType_SPLINE:
            nvSplineConstraint_solve(cons);
            break;
    }
}
