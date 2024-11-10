/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/constraints/distance_constraint.h"
#include "khg_phy/space.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>


/**
 * @file constraints/distance_constraint.c
 * 
 * @brief Distance constraint solver.
 */


phy_constraint *phy_distance_constraint_new(phy_distance_constraint_initializer init) {
    if (init.length < 0.0) {
      utl_error_func("Distance constraint length is negative", utl_user_defined_data);
      return NULL;
    }

    phy_constraint *cons = malloc(sizeof(phy_constraint));
    if (!cons) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }

    if (!init.a && !init.b) {
      utl_error_func("Both bodies are null", utl_user_defined_data);
      free(cons);
      return NULL;
    }

    cons->a = init.a;
    cons->b = init.b;
    cons->type = PHY_CONSTRAINT_TYPE_DISTANCE;
    cons->ignore_collision = false;

    cons->def = (phy_distance_constraint *)malloc(sizeof(phy_distance_constraint));
    if (!cons->def) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
      free(cons);
      return NULL; 
    }
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;

    dist_cons->length = init.length;
    dist_cons->anchor_a = init.anchor_a;
    dist_cons->anchor_b = init.anchor_b;
    dist_cons->max_force = init.max_force;
    dist_cons->spring = init.spring;
    dist_cons->hertz = init.hertz;
    dist_cons->damping = init.damping;

    dist_cons->xanchor_a = phy_vector2_zero;
    dist_cons->xanchor_b = phy_vector2_zero;
    dist_cons->normal = phy_vector2_zero;
    dist_cons->bias = 0.0;
    dist_cons->mass = 0.0;
    dist_cons->impulse = 0.0;
    dist_cons->max_impulse = 0.0;
    dist_cons->bias_rate = 0.0;
    dist_cons->mass_coeff = 0.0;
    dist_cons->impulse_coeff = 0.0;

    return cons;
}

phy_rigid_body *phy_distance_constraint_get_body_a(const phy_constraint *cons) {
    return cons->a;
}

phy_rigid_body *phy_distance_constraint_get_body_b(const phy_constraint *cons) {
    return cons->b;
}

void phy_distance_constraint_set_length(phy_constraint *cons, float length) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    dist_cons->length = length;
}

float phy_distance_constraint_get_length(const phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    return dist_cons->length;
}

void phy_distance_constraint_set_anchor_a(phy_constraint *cons, phy_vector2 anchor_a) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    dist_cons->anchor_a = anchor_a;
}

phy_vector2 phy_distance_constraint_get_anchor_a(const phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    return dist_cons->anchor_a;
}

void phy_distance_constraint_set_anchor_b(phy_constraint *cons, phy_vector2 anchor_b) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    dist_cons->anchor_b = anchor_b;
}

phy_vector2 phy_distance_constraint_get_anchor_b(const phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    return dist_cons->anchor_b;
}

void phy_distance_constraint_set_max_force(phy_constraint *cons, float max_force) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    dist_cons->max_force = max_force;
}

float phy_distance_constraint_get_max_force(const phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    return dist_cons->max_force;
}

void phy_distance_constraint_set_spring(phy_constraint *cons, bool spring) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    dist_cons->spring = spring;
}

bool phy_distance_constraint_get_spring(const phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    return dist_cons->spring;
}

void phy_distance_constraint_set_hertz(phy_constraint *cons, float hertz) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    dist_cons->hertz = hertz;
}

float phy_distance_constraint_get_hertz(const phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    return dist_cons->hertz;
}

void phy_distance_constraint_set_damping(phy_constraint *cons, float damping) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    dist_cons->damping = damping;
}

float phy_distance_constraint_get_damping(const phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    return dist_cons->damping;
}

void phy_distance_constraint_presolve(
    phy_space *space,
    phy_constraint *cons,
    float dt,
    float inv_dt
) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    phy_rigid_body *a = cons->a;
    phy_rigid_body *b = cons->b;

    // Transformed anchor points
    phy_vector2 rpa, rpb;
    float invmass_a, invmass_b, invinertia_a, invinertia_b;

    // If a body is NULL count them as static bodies

    if (!a) {
        dist_cons->xanchor_a = phy_vector2_zero;
        rpa = dist_cons->anchor_a;
        invmass_a = invinertia_a = 0.0;
    } else {
        dist_cons->xanchor_a = phy_vector2_rotate(dist_cons->anchor_a, a->angle);
        rpa = phy_vector2_add(dist_cons->xanchor_a, a->position);
        invmass_a = a->invmass;
        invinertia_a = a->invinertia;
    }

    if (!b) {
        dist_cons->xanchor_b = phy_vector2_zero;
        rpb = dist_cons->anchor_b;
        invmass_b = invinertia_b = 0.0;
    } else {
        dist_cons->xanchor_b = phy_vector2_rotate(dist_cons->anchor_b, b->angle);
        rpb = phy_vector2_add(dist_cons->xanchor_b, b->position);
        invmass_b = b->invmass;
        invinertia_b = b->invinertia;
    }

    phy_vector2 delta = phy_vector2_sub(rpb, rpa);
    dist_cons->normal = phy_vector2_normalize(delta);
    float offset = phy_vector2_len(delta) - dist_cons->length;

    // Baumgarte stabilization bias
    dist_cons->bias = space->settings.baumgarte * inv_dt * offset;

    // Constraint effective mass
    dist_cons->mass = 1.0 / phy_calc_mass_k(
        dist_cons->normal,
        dist_cons->xanchor_a, dist_cons->xanchor_b,
        invmass_a, invmass_b,
        invinertia_a, invinertia_b
    );

    dist_cons->max_impulse = dist_cons->max_force * dt;

    /*
        Soft-constraint formulation
        https://box2d.org/files/ErinCatto_SoftConstraints_GDC2011.pdf
        https://box2d.org/posts/2024/02/solver2d/
    */
    if (dist_cons->spring) {
        float zeta = dist_cons->damping;
        float omega = 2.0 * PHY_PI * dist_cons->hertz;
        float a1 = 2.0 * zeta + omega * (1.0 / inv_dt);
        float a2 = (1.0 / inv_dt) * omega * a1;
        float a3 = 1.0 / (1.0 + a2);
        dist_cons->bias_rate = omega / a1;
        dist_cons->mass_coeff = a2 * a3;
        dist_cons->impulse_coeff = a3;
    }
    else {
        dist_cons->bias_rate = 1.0;
        dist_cons->mass_coeff = 1.0;
        dist_cons->impulse_coeff = 0.0;
    }
}

void phy_distance_constraint_warmstart(phy_space *space, phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    phy_rigid_body *a = cons->a;
    phy_rigid_body *b = cons->b;

    if (space->settings.warmstarting) {
        phy_vector2 impulse = phy_vector2_mul(dist_cons->normal, dist_cons->impulse);

        if (a) phy_rigid_body_apply_impulse(cons->a, phy_vector2_neg(impulse), dist_cons->xanchor_a);
        if (b) phy_rigid_body_apply_impulse(cons->b, impulse, dist_cons->xanchor_b);
    }
    else {
        dist_cons->impulse = 0.0;
    }
}

void phy_distance_constraint_solve(phy_constraint *cons) {
    phy_distance_constraint *dist_cons = (phy_distance_constraint *)cons->def;
    phy_rigid_body *a = cons->a;
    phy_rigid_body *b = cons->b;

    phy_vector2 linear_velocity_a, linear_velocity_b;
    float angular_velocity_a, angular_velocity_b;

    if (!a) {
        linear_velocity_a = phy_vector2_zero;
        angular_velocity_a = 0.0;
    } else {
        linear_velocity_a = a->linear_velocity;
        angular_velocity_a = a->angular_velocity;
    }

    if (!b) {
        linear_velocity_b = phy_vector2_zero;
        angular_velocity_b = 0.0;
    } else {
        linear_velocity_b = b->linear_velocity;
        angular_velocity_b = b->angular_velocity;
    }

    phy_vector2 rv = phy_calc_relative_velocity(
        linear_velocity_a, angular_velocity_a, dist_cons->xanchor_a,
        linear_velocity_b, angular_velocity_b, dist_cons->xanchor_b
    );

    float vn = phy_vector2_dot(rv, dist_cons->normal);

    // Constraint impulse magnitude
    float lambda = (dist_cons->bias * dist_cons->bias_rate + vn);
    lambda *= dist_cons->mass * -dist_cons->mass_coeff;
    lambda -= dist_cons->impulse_coeff * dist_cons->impulse;

    // Accumulate impulse
    float limit = dist_cons->max_impulse;
    float lambda0 = dist_cons->impulse;
    dist_cons->impulse = phy_fclamp(lambda0 + lambda, -limit, limit);
    lambda = dist_cons->impulse - lambda0;

    phy_vector2 impulse = phy_vector2_mul(dist_cons->normal, lambda);

    // Apply constraint impulse
    if (a) phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), dist_cons->xanchor_a);
    if (b) phy_rigid_body_apply_impulse(b, impulse, dist_cons->xanchor_b);
}
