/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/constraints/contact_constraint.h"
#include "khg_phy/vector.h"
#include "khg_phy/math.h"
#include "khg_phy/space.h"


/**
 * @file constraints/contact_constraint.c
 * 
 * @brief Contact constraint solver functions.
 */


void phy_contact_presolve(
    phy_space *space,
    phy_persistent_contact_pair *pcp,
    float inv_dt
) {

    phy_rigid_body *a = pcp->body_a;
    phy_rigid_body *b = pcp->body_b;
    phy_vector2 normal = pcp->normal;
    phy_vector2 tangent = phy_vector2_perpr(normal);

    // Mixed restitution
    float e = phy_mix_coefficients(
        a->material.restitution,
        b->material.restitution,
        space->settings.restitution_mix
    );

    // Mixed friction
    float friction = phy_mix_coefficients(
        a->material.friction,
        b->material.friction,
        space->settings.friction_mix
    );

    for (size_t i = 0; i < pcp->contact_count; i++) {
        phy_contact *contact = &pcp->contacts[i];
        if (contact->separation > 0.0) continue;
        phy_contact_solver_info *solver_info = &contact->solver_info;

        solver_info->friction = friction;

        // Relative velocity at contact
        phy_vector2 rv = phy_calc_relative_velocity(
            a->linear_velocity, a->angular_velocity, contact->anchor_a,
            b->linear_velocity, b->angular_velocity, contact->anchor_b
        );

        // Restitution * normal velocity at first impact
        float vn = phy_vector2_dot(rv, normal);

        // Restitution bias
        solver_info->velocity_bias = 0.0;
        if (vn < -1.0) {
            solver_info->velocity_bias = e * vn;
        }

        // Effective masses
        solver_info->mass_normal = 1.0 / phy_calc_mass_k(
            normal,
            contact->anchor_a, contact->anchor_b,
            a->invmass, b->invmass,
            a->invinertia, b->invinertia
        );
        solver_info->mass_tangent = 1.0 / phy_calc_mass_k(
            tangent,
            contact->anchor_a, contact->anchor_b,
            a->invmass, b->invmass,
            a->invinertia, b->invinertia
        );

        if (space->settings.contact_position_correction == PHY_CONTACT_POSITION_CORRECTION_BAUMGARTE) {
            // Position error is fed back to the velocity constraint as a bias value
            float correction = fminf(contact->separation + space->settings.penetration_slop, 0.0);
            solver_info->position_bias = space->settings.baumgarte * inv_dt * correction;

            // Perfect restitution + baumgarte leads to overshooting
            if (solver_info->velocity_bias < solver_info->position_bias)
                solver_info->velocity_bias -= solver_info->position_bias;
        }
        else if (space->settings.contact_position_correction == PHY_CONTACT_POSITION_CORRECTION_NGS) {
        }
    }

}

void phy_contact_warmstart(phy_space *space, phy_persistent_contact_pair *pcp) {

    phy_rigid_body *a = pcp->body_a;
    phy_rigid_body *b = pcp->body_b;
    phy_vector2 normal = pcp->normal;
    phy_vector2 tangent = phy_vector2_perpr(normal);

    for (size_t i = 0; i < pcp->contact_count; i++) {
        phy_contact *contact = &pcp->contacts[i];
        if (contact->separation > 0.0) continue;
        // No need to apply warmstarting if this contact is just created
        if (!contact->is_persisted) continue;
        phy_contact_solver_info *solver_info = &contact->solver_info;

        if (space->settings.warmstarting) {
            phy_vector2 impulse = phy_vector2_add(
                phy_vector2_mul(normal, solver_info->normal_impulse),
                phy_vector2_mul(tangent, solver_info->tangent_impulse)
            );
            
            phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), contact->anchor_a);
            phy_rigid_body_apply_impulse(b, impulse, contact->anchor_b);
        }
        else {
            solver_info->normal_impulse = 0.0;
            solver_info->tangent_impulse = 0.0;
        }
    }

}

void phy_contact_solve_velocity(phy_persistent_contact_pair *pcp) {

    phy_rigid_body *a = pcp->body_a;
    phy_rigid_body *b = pcp->body_b;
    phy_vector2 normal = pcp->normal;
    phy_vector2 tangent = phy_vector2_perpr(normal);

    /*
        In an iterative solver what is applied the last affects the result more.
        So we solve normal impulse after tangential impulse because
        non-penetration is more important.
    */

    // Solve friction
    for (size_t i = 0; i < pcp->contact_count; i++) {
        phy_contact *contact = &pcp->contacts[i];
        //if (contact->separation > 0.0) continue;
        phy_contact_solver_info *solver_info = &contact->solver_info;

        // Don't bother calculating friction if the coefficent is 0
        if (solver_info->friction == 0.0) continue;

        // Relative velocity at contact
        phy_vector2 rv = phy_calc_relative_velocity(
            a->linear_velocity, a->angular_velocity, contact->anchor_a,
            b->linear_velocity, b->angular_velocity, contact->anchor_b
        );

        // Tangential impulse magnitude
        float lambda = -phy_vector2_dot(rv, tangent) * solver_info->mass_tangent;

        // Accumulate tangential impulse
        float f = solver_info->normal_impulse * solver_info->friction;
        float lambda0 = solver_info->tangent_impulse;
        // Clamp lambda between friction limits
        solver_info->tangent_impulse = fmaxf(-f, fminf(lambda0 + lambda, f));
        lambda = solver_info->tangent_impulse - lambda0;

        phy_vector2 impulse = phy_vector2_mul(tangent, lambda);

        // Apply tangential impulse
        phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), contact->anchor_a);
        phy_rigid_body_apply_impulse(b, impulse, contact->anchor_b);
    }

    // Solve penetration
    for (size_t i = 0; i < pcp->contact_count; i++) {
        phy_contact *contact = &pcp->contacts[i];
        if (contact->separation > 0.0) continue;
        phy_contact_solver_info *solver_info = &contact->solver_info;

        // Relative velocity at contact
        phy_vector2 rv = phy_calc_relative_velocity(
            a->linear_velocity, a->angular_velocity, contact->anchor_a,
            b->linear_velocity, b->angular_velocity, contact->anchor_b
        );

        float vn = phy_vector2_dot(rv, normal);

        // Normal impulse magnitude
        float lambda = -(vn + solver_info->velocity_bias + solver_info->position_bias);
        lambda *= solver_info->mass_normal;

        // Accumulate normal impulse
        float lambda0 = solver_info->normal_impulse;
        // Clamp lambda because we only want to solve penetration
        solver_info->normal_impulse = fmaxf(lambda0 + lambda, 0.0);
        lambda = solver_info->normal_impulse - lambda0;

        phy_vector2 impulse = phy_vector2_mul(normal, lambda);

        // Apply normal impulse
        phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), contact->anchor_a);
        phy_rigid_body_apply_impulse(b, impulse, contact->anchor_b);
    }

}

void phy_contact_solve_position(phy_persistent_contact_pair *pcp) {
    // TODO: Finish the NGS iterations early if there is no collision?


}
