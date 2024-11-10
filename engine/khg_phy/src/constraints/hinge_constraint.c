/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/constraints/hinge_constraint.h"
#include "khg_phy/space.h"
#include "khg_utl/error_func.h"


/**
 * @file constraints/hinge_constraint.c
 * 
 * @brief Hinge constraint solver.
 */


phy_constraint *phy_hinge_constraint_new(phy_hinge_constraint_initializer init) {
    phy_constraint *cons = NV_NEW(phy_constraint);
    if (!cons) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }

    if (!init.a && !init.b) {
      utl_error_func("Both bodies are null", utl_user_defined_data);
      NV_FREE(cons);
      return NULL;
    }

    cons->a = init.a;
    cons->b = init.b;
    cons->type = PHY_CONSTRAINT_TYPE_HINGE;
    cons->ignore_collision = false;

    cons->def = NV_NEW(phy_hinge_constraint);
    if (!cons->def) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
      NV_FREE(cons);
      return NULL; 
    }
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;

    hinge_cons->anchor = init.anchor;
    hinge_cons->enable_limits = init.enable_limits;
    hinge_cons->lower_limit = init.lower_limit;
    hinge_cons->upper_limit = init.upper_limit;
    hinge_cons->max_force = init.max_force;
    hinge_cons->angle = 0.0;

    nv_float angle_a, angle_b;
    if (init.a) {
        hinge_cons->anchor_a = nvVector2_sub(init.anchor, init.a->position);
        angle_a = init.a->angle;
    }
    else {
        hinge_cons->anchor_a = init.anchor;
        angle_a = 0.0;
    }
    if (init.b) {
        hinge_cons->anchor_b = nvVector2_sub(init.anchor, init.b->position);
        angle_b = init.b->angle;
    }
    else {
        hinge_cons->anchor_b = init.anchor;
        angle_b = 0.0;
    }

    hinge_cons->reference_angle = angle_b - angle_a;
    hinge_cons->lower_impulse = 0.0;
    hinge_cons->upper_impulse = 0.0;
    hinge_cons->lower_bias = 0.0;
    hinge_cons->upper_bias = 0.0;
    hinge_cons->axial_mass = 0.0;
    hinge_cons->xanchor_a = nvVector2_zero;
    hinge_cons->xanchor_b = nvVector2_zero;
    hinge_cons->normal = nvVector2_zero;
    hinge_cons->bias = 0.0;
    hinge_cons->mass = 0.0;
    hinge_cons->impulse = 0.0;
    hinge_cons->max_impulse = 0.0;

    return cons;
}

phy_rigid_body *phy_hinge_constraint_get_body_a(const phy_constraint *cons) {
    return cons->a;
}

phy_rigid_body *phy_hinge_constraint_get_body_b(const phy_constraint *cons) {
    return cons->b;
}

void phy_hinge_constraint_set_anchor(phy_constraint *cons, phy_vector2 anchor) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    hinge_cons->anchor = anchor;

    if (cons->a) {
        hinge_cons->anchor_a = nvVector2_sub(hinge_cons->anchor, cons->a->position);
    }
    else {
        hinge_cons->anchor_a = hinge_cons->anchor;
    }
    if (cons->b) {
        hinge_cons->anchor_b = nvVector2_sub(hinge_cons->anchor, cons->b->position);
    }
    else {
        hinge_cons->anchor_b = hinge_cons->anchor;
    }
}

phy_vector2 phy_hinge_constraint_get_anchor(const phy_constraint *cons) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    return hinge_cons->anchor;
}

void phy_hinge_constraint_set_limits(phy_constraint *cons, bool limits) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    hinge_cons->enable_limits = limits;
}

bool phy_hinge_constraint_get_limits(const phy_constraint *cons) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    return hinge_cons->enable_limits;
}

void phy_hinge_constraint_set_upper_limit(phy_constraint *cons, nv_float upper_limit) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    hinge_cons->upper_limit = upper_limit;
}

float phy_hinge_constraint_get_upper_limit(const phy_constraint *cons) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    return hinge_cons->upper_limit;
}

void phy_hinge_constraint_set_lower_limit(phy_constraint *cons, float lower_limit) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    hinge_cons->lower_limit = lower_limit;
}

float phy_hinge_constraint_get_lower_limit(const phy_constraint *cons) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    return hinge_cons->lower_limit;
}

void phy_hinge_constraint_set_max_force(phy_constraint *cons, float max_force) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    hinge_cons->max_force = max_force;
}

float phy_hinge_constraint_get_max_force(const phy_constraint *cons) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    return hinge_cons->max_force;
}

void phy_hinge_constraint_presolve(
    phy_space *space,
    phy_constraint *cons,
    float dt,
    float inv_dt
) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    phy_rigid_body *a = cons->a;
    phy_rigid_body *b = cons->b;

    // Transformed anchor points
    phy_vector2 rpa, rpb;
    nv_float invmass_a, invmass_b, invinertia_a, invinertia_b;

    // If a body is NULL count them as static bodies

    if (!a) {
        hinge_cons->xanchor_a = nvVector2_zero;
        rpa = hinge_cons->anchor_a;
        invmass_a = invinertia_a = 0.0;
    } else {
        hinge_cons->xanchor_a = nvVector2_rotate(hinge_cons->anchor_a, a->angle);
        rpa = nvVector2_add(hinge_cons->xanchor_a, a->position);
        invmass_a = a->invmass;
        invinertia_a = a->invinertia;
    }

    if (!b) {
        hinge_cons->xanchor_b = nvVector2_zero;
        rpb = hinge_cons->anchor_b;
        invmass_b = invinertia_b = 0.0;
    } else {
        hinge_cons->xanchor_b = nvVector2_rotate(hinge_cons->anchor_b, b->angle);
        rpb = nvVector2_add(hinge_cons->xanchor_b, b->position);
        invmass_b = b->invmass;
        invinertia_b = b->invinertia;
    }

    // If delta is 0 point constraint is ensured
    phy_vector2 delta = nvVector2_sub(rpb, rpa);
    if (nvVector2_len2(delta) == 0.0) hinge_cons->normal = nvVector2_zero;
    else hinge_cons->normal = nvVector2_normalize(delta);
    nv_float offset = nvVector2_len(delta);

    // Baumgarte position correction bias
    hinge_cons->bias = space->settings.baumgarte * inv_dt * offset;

    // Point constraint effective mass
    hinge_cons->mass = 1.0 / nv_calc_mass_k(
        hinge_cons->normal,
        hinge_cons->xanchor_a, hinge_cons->xanchor_b,
        invmass_a, invmass_b,
        invinertia_a, invinertia_b
    );

    hinge_cons->max_impulse = hinge_cons->max_force * dt;

    hinge_cons->axial_mass = 1.0 / (invinertia_a + invinertia_b);

    nv_float angle_a, angle_b;
    if (a) angle_a = a->angle;
    else angle_a = 0.0;
    if (b) angle_b = b->angle;
    else angle_b = 0.0;

    hinge_cons->angle = angle_b - angle_a - hinge_cons->reference_angle;

    // Angular limit constraints
    // C = θb - θa - θr - θl
    // Cdot = wb - wa
    // Jacobian = [1, -1]

    nv_float lower_c = hinge_cons->angle - hinge_cons->lower_limit;
    hinge_cons->lower_bias = nv_fmax(lower_c, 0.0) * space->settings.baumgarte * inv_dt;

    nv_float upper_c = hinge_cons->upper_limit - hinge_cons->angle;
    hinge_cons->upper_bias = nv_fmax(upper_c, 0.0) * 0.2 * inv_dt;
}

void phy_hinge_constraint_warmstart(phy_space *space, phy_constraint *cons) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    phy_rigid_body *a = cons->a;
    phy_rigid_body *b = cons->b;

    if (space->settings.warmstarting) {
        phy_vector2 impulse = nvVector2_mul(hinge_cons->normal, hinge_cons->impulse);
        nv_float axial_impulse = hinge_cons->lower_impulse - hinge_cons->upper_impulse;

        if (a) {
            nvRigidBody_apply_impulse(a, nvVector2_neg(impulse), hinge_cons->xanchor_a);
            a->angular_velocity -= a->invinertia * axial_impulse;
        } 
        if (b) {
            nvRigidBody_apply_impulse(b, impulse, hinge_cons->xanchor_b);
            b->angular_velocity += b->invinertia * axial_impulse;
        }
        
    }
    else {
        hinge_cons->impulse = 0.0;
        hinge_cons->upper_impulse = 0.0;
        hinge_cons->lower_impulse = 0.0;
    }
}

void phy_hinge_constraint_solve(phy_constraint *cons, nv_float inv_dt) {
    phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
    phy_rigid_body *a = cons->a;
    phy_rigid_body *b = cons->b;

    // Solve angular limits
    if (hinge_cons->enable_limits) {
        nv_float cdot, wa, wb, lambda, lambda0;

        if (a) wa = a->angular_velocity;
        else wa = 0.0;
        if (b) wb = b->angular_velocity;
        else wb = 0.0;

        // TODO: Calculate angular limit errors in presolve?

        // Solve lower limit
        cdot = wb - wa;
        lambda = (cdot + hinge_cons->lower_bias) * -hinge_cons->axial_mass;

        // Accumulate lower impulse
        lambda0 = hinge_cons->lower_impulse;
        hinge_cons->lower_impulse = nv_fmax(hinge_cons->lower_impulse + lambda, 0.0);
        lambda = hinge_cons->lower_impulse - lambda0;

        // Apply lower impulse
        if (a) a->angular_velocity -= lambda * a->invinertia;
        if (b) b->angular_velocity += lambda * b->invinertia;

        // Solve upper limit
        cdot = wa - wb;
        lambda = (cdot + hinge_cons->upper_bias) * -hinge_cons->axial_mass;

        // Accumulate upper impulse
        lambda0 = hinge_cons->upper_impulse;
        hinge_cons->upper_impulse = nv_fmax(hinge_cons->upper_impulse + lambda, 0.0);
        lambda = hinge_cons->upper_impulse - lambda0;

        // Apply upper impulse
        if (a) a->angular_velocity += lambda * a->invinertia;
        if (b) b->angular_velocity -= lambda * b->invinertia;
    }

    // Solve point constraint
    // TODO: Skip if point constraint is ensured?

    phy_vector2 linear_velocity_a, linear_velocity_b;
    nv_float angular_velocity_a, angular_velocity_b;

    if (!a) {
        linear_velocity_a = nvVector2_zero;
        angular_velocity_a = 0.0;
    } else {
        linear_velocity_a = a->linear_velocity;
        angular_velocity_a = a->angular_velocity;
    }

    if (!b) {
        linear_velocity_b = nvVector2_zero;
        angular_velocity_b = 0.0;
    } else {
        linear_velocity_b = b->linear_velocity;
        angular_velocity_b = b->angular_velocity;
    }

    phy_vector2 rv = nv_calc_relative_velocity(
        linear_velocity_a, angular_velocity_a, hinge_cons->xanchor_a,
        linear_velocity_b, angular_velocity_b, hinge_cons->xanchor_b
    );

    nv_float vn = nvVector2_dot(rv, hinge_cons->normal);

    // Point constraint impulse magnitude
    nv_float lambda = -(hinge_cons->bias + vn) * hinge_cons->mass;

    // Accumulate impulse
    nv_float limit = hinge_cons->max_impulse;
    nv_float lambda0 = hinge_cons->impulse;
    hinge_cons->impulse = nv_fclamp(lambda0 + lambda, -limit, limit);
    lambda = hinge_cons->impulse - lambda0;

    phy_vector2 impulse = nvVector2_mul(hinge_cons->normal, lambda);

    // Apply point constraint impulse
    if (a) nvRigidBody_apply_impulse(a, nvVector2_neg(impulse), hinge_cons->xanchor_a);
    if (b) nvRigidBody_apply_impulse(b, impulse, hinge_cons->xanchor_b);
}
