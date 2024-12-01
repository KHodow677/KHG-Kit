#include "khg_phy/math.h"
#include "khg_phy/space.h"
#include "khg_phy/constraints/contact_constraint.h"
#include "khg_phy/core/phy_vector.h"

void phy_contact_presolve(phy_space *space, phy_persistent_contact_pair *pcp, float inv_dt) {
  phy_rigid_body *a = pcp->body_a;
  phy_rigid_body *b = pcp->body_b;
  phy_vector2 normal = pcp->normal;
  phy_vector2 tangent = phy_vector2_perpr(normal);
  float e = phy_mix_coefficients(a->material.restitution, b->material.restitution, space->settings.restitution_mix);
  float friction = phy_mix_coefficients(a->material.friction, b->material.friction, space->settings.friction_mix);
  for (size_t i = 0; i < pcp->contact_count; i++) {
    phy_contact *contact = &pcp->contacts[i];
    if (contact->separation > 0.0) {
      continue;
    }
    phy_contact_solver_info *solver_info = &contact->solver_info;
    solver_info->friction = friction;
    phy_vector2 rv = phy_calc_relative_velocity(a->linear_velocity, a->angular_velocity, contact->anchor_a, b->linear_velocity, b->angular_velocity, contact->anchor_b);
    float vn = phy_vector2_dot(rv, normal);
    solver_info->velocity_bias = 0.0;
    if (vn < -1.0) {
      solver_info->velocity_bias = e * vn;
    }
    solver_info->mass_normal = 1.0 / phy_calc_mass_k(normal, contact->anchor_a, contact->anchor_b, a->invmass, b->invmass, a->invinertia, b->invinertia);
    solver_info->mass_tangent = 1.0 / phy_calc_mass_k(tangent, contact->anchor_a, contact->anchor_b, a->invmass, b->invmass, a->invinertia, b->invinertia);
    if (space->settings.contact_position_correction == PHY_CONTACT_POSITION_CORRECTION_BAUMGARTE) {
      float correction = fminf(contact->separation + space->settings.penetration_slop, 0.0);
      solver_info->position_bias = space->settings.baumgarte * inv_dt * correction;
      if (solver_info->velocity_bias < solver_info->position_bias) {
        solver_info->velocity_bias -= solver_info->position_bias;
      }
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
    if (contact->separation > 0.0) {
      continue;
    }
    if (!contact->is_persisted) {
      continue;
    }
    phy_contact_solver_info *solver_info = &contact->solver_info;
    if (space->settings.warmstarting) {
      phy_vector2 impulse = phy_vector2_add(phy_vector2_mul(normal, solver_info->normal_impulse), phy_vector2_mul(tangent, solver_info->tangent_impulse));
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
  for (size_t i = 0; i < pcp->contact_count; i++) {
    phy_contact *contact = &pcp->contacts[i];
    phy_contact_solver_info *solver_info = &contact->solver_info;
    if (solver_info->friction == 0.0) {
      continue;
    }
    phy_vector2 rv = phy_calc_relative_velocity(a->linear_velocity, a->angular_velocity, contact->anchor_a, b->linear_velocity, b->angular_velocity, contact->anchor_b);
    float lambda = -phy_vector2_dot(rv, tangent) * solver_info->mass_tangent;
    float f = solver_info->normal_impulse * solver_info->friction;
    float lambda0 = solver_info->tangent_impulse;
    solver_info->tangent_impulse = fmaxf(-f, fminf(lambda0 + lambda, f));
    lambda = solver_info->tangent_impulse - lambda0;
    phy_vector2 impulse = phy_vector2_mul(tangent, lambda);
    phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), contact->anchor_a);
    phy_rigid_body_apply_impulse(b, impulse, contact->anchor_b);
  }
  for (size_t i = 0; i < pcp->contact_count; i++) {
    phy_contact *contact = &pcp->contacts[i];
    if (contact->separation > 0.0) {
      continue;
    }
    phy_contact_solver_info *solver_info = &contact->solver_info;
    phy_vector2 rv = phy_calc_relative_velocity(a->linear_velocity, a->angular_velocity, contact->anchor_a, b->linear_velocity, b->angular_velocity, contact->anchor_b);
    float vn = phy_vector2_dot(rv, normal);
    float lambda = -(vn + solver_info->velocity_bias + solver_info->position_bias);
    lambda *= solver_info->mass_normal;
    float lambda0 = solver_info->normal_impulse;
    solver_info->normal_impulse = fmaxf(lambda0 + lambda, 0.0);
    lambda = solver_info->normal_impulse - lambda0;
    phy_vector2 impulse = phy_vector2_mul(normal, lambda);
    phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), contact->anchor_a);
    phy_rigid_body_apply_impulse(b, impulse, contact->anchor_b);
  }
}

void phy_contact_solve_position(phy_persistent_contact_pair *pcp) {
}

