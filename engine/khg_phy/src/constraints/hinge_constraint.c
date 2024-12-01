#include "khg_phy/constraints/hinge_constraint.h"
#include "khg_phy/space.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

phy_constraint *phy_hinge_constraint_new(phy_hinge_constraint_initializer init) {
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
  cons->type = PHY_CONSTRAINT_TYPE_HINGE;
  cons->ignore_collision = false;
  cons->def = (phy_hinge_constraint *)malloc(sizeof(phy_hinge_constraint));
  if (!cons->def) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
    free(cons);
    return NULL; 
  }
  phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
  hinge_cons->anchor = init.anchor;
  hinge_cons->enable_limits = init.enable_limits;
  hinge_cons->lower_limit = init.lower_limit;
  hinge_cons->upper_limit = init.upper_limit;
  hinge_cons->max_force = init.max_force;
  hinge_cons->angle = 0.0;
  float angle_a, angle_b;
  if (init.a) {
    hinge_cons->anchor_a = phy_vector2_sub(init.anchor, init.a->position);
    angle_a = init.a->angle;
  }
  else {
    hinge_cons->anchor_a = init.anchor;
    angle_a = 0.0;
  }
  if (init.b) {
    hinge_cons->anchor_b = phy_vector2_sub(init.anchor, init.b->position);
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
  hinge_cons->xanchor_a = phy_vector2_zero;
  hinge_cons->xanchor_b = phy_vector2_zero;
  hinge_cons->normal = phy_vector2_zero;
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
    hinge_cons->anchor_a = phy_vector2_sub(hinge_cons->anchor, cons->a->position);
  }
  else {
    hinge_cons->anchor_a = hinge_cons->anchor;
  }
  if (cons->b) {
    hinge_cons->anchor_b = phy_vector2_sub(hinge_cons->anchor, cons->b->position);
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

void phy_hinge_constraint_set_upper_limit(phy_constraint *cons, float upper_limit) {
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

void phy_hinge_constraint_presolve(phy_space *space, phy_constraint *cons, float dt, float inv_dt) {
  phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
  phy_rigid_body *a = cons->a;
  phy_rigid_body *b = cons->b;
  phy_vector2 rpa, rpb;
  float invmass_a, invmass_b, invinertia_a, invinertia_b;
  if (!a) {
    hinge_cons->xanchor_a = phy_vector2_zero;
    rpa = hinge_cons->anchor_a;
    invmass_a = invinertia_a = 0.0;
  } 
  else {
    hinge_cons->xanchor_a = phy_vector2_rotate(hinge_cons->anchor_a, a->angle);
    rpa = phy_vector2_add(hinge_cons->xanchor_a, a->position);
    invmass_a = a->invmass;
    invinertia_a = a->invinertia;
  }
  if (!b) {
    hinge_cons->xanchor_b = phy_vector2_zero;
    rpb = hinge_cons->anchor_b;
    invmass_b = invinertia_b = 0.0;
  } 
  else {
    hinge_cons->xanchor_b = phy_vector2_rotate(hinge_cons->anchor_b, b->angle);
    rpb = phy_vector2_add(hinge_cons->xanchor_b, b->position);
    invmass_b = b->invmass;
    invinertia_b = b->invinertia;
  }
  phy_vector2 delta = phy_vector2_sub(rpb, rpa);
  if (phy_vector2_len2(delta) == 0.0) {
    hinge_cons->normal = phy_vector2_zero;
  }
  else {
    hinge_cons->normal = phy_vector2_normalize(delta);
  }
  float offset = phy_vector2_len(delta);
  hinge_cons->bias = space->settings.baumgarte * inv_dt * offset;
  hinge_cons->mass = 1.0 / phy_calc_mass_k(hinge_cons->normal, hinge_cons->xanchor_a, hinge_cons->xanchor_b, invmass_a, invmass_b, invinertia_a, invinertia_b);
  hinge_cons->max_impulse = hinge_cons->max_force * dt;
  hinge_cons->axial_mass = 1.0 / (invinertia_a + invinertia_b);
  float angle_a, angle_b;
  if (a) {
    angle_a = a->angle;
  }
  else {
    angle_a = 0.0;
  }
  if (b) {
    angle_b = b->angle;
  }
  else {
    angle_b = 0.0;
  }
  hinge_cons->angle = angle_b - angle_a - hinge_cons->reference_angle;
  float lower_c = hinge_cons->angle - hinge_cons->lower_limit;
  hinge_cons->lower_bias = fmaxf(lower_c, 0.0) * space->settings.baumgarte * inv_dt;
  float upper_c = hinge_cons->upper_limit - hinge_cons->angle;
  hinge_cons->upper_bias = fmaxf(upper_c, 0.0) * 0.2 * inv_dt;
}

void phy_hinge_constraint_warmstart(phy_space *space, phy_constraint *cons) {
  phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
  phy_rigid_body *a = cons->a;
  phy_rigid_body *b = cons->b;
  if (space->settings.warmstarting) {
    phy_vector2 impulse = phy_vector2_mul(hinge_cons->normal, hinge_cons->impulse);
    float axial_impulse = hinge_cons->lower_impulse - hinge_cons->upper_impulse;
    if (a) {
      phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), hinge_cons->xanchor_a);
      a->angular_velocity -= a->invinertia * axial_impulse;
    } 
    if (b) {
      phy_rigid_body_apply_impulse(b, impulse, hinge_cons->xanchor_b);
      b->angular_velocity += b->invinertia * axial_impulse;
    }
  }
  else {
    hinge_cons->impulse = 0.0;
    hinge_cons->upper_impulse = 0.0;
    hinge_cons->lower_impulse = 0.0;
  }
}

void phy_hinge_constraint_solve(phy_constraint *cons, float inv_dt) {
  phy_hinge_constraint *hinge_cons = (phy_hinge_constraint *)cons->def;
  phy_rigid_body *a = cons->a;
  phy_rigid_body *b = cons->b;
  if (hinge_cons->enable_limits) {
    float cdot, wa, wb, lambda, lambda0;
    if (a) {
      wa = a->angular_velocity;
    }
    else {
      wa = 0.0;
    }
    if (b) {
      wb = b->angular_velocity;
    }
    else {
      wb = 0.0;
    }
    cdot = wb - wa;
    lambda = (cdot + hinge_cons->lower_bias) * -hinge_cons->axial_mass;
    lambda0 = hinge_cons->lower_impulse;
    hinge_cons->lower_impulse = fmaxf(hinge_cons->lower_impulse + lambda, 0.0);
    lambda = hinge_cons->lower_impulse - lambda0;
    if (a) {
      a->angular_velocity -= lambda * a->invinertia;
    }
    if (b) {
      b->angular_velocity += lambda * b->invinertia;
    }
    cdot = wa - wb;
    lambda = (cdot + hinge_cons->upper_bias) * -hinge_cons->axial_mass;
    lambda0 = hinge_cons->upper_impulse;
    hinge_cons->upper_impulse = fmaxf(hinge_cons->upper_impulse + lambda, 0.0);
    lambda = hinge_cons->upper_impulse - lambda0;
    if (a) {
      a->angular_velocity += lambda * a->invinertia;
    }
    if (b)  {
      b->angular_velocity -= lambda * b->invinertia;
    }
  }
  phy_vector2 linear_velocity_a, linear_velocity_b;
  float angular_velocity_a, angular_velocity_b;
  if (!a) {
    linear_velocity_a = phy_vector2_zero;
    angular_velocity_a = 0.0;
  } 
  else {
    linear_velocity_a = a->linear_velocity;
    angular_velocity_a = a->angular_velocity;
  }
  if (!b) {
    linear_velocity_b = phy_vector2_zero;
    angular_velocity_b = 0.0;
  } 
  else {
    linear_velocity_b = b->linear_velocity;
    angular_velocity_b = b->angular_velocity;
  }
  phy_vector2 rv = phy_calc_relative_velocity(linear_velocity_a, angular_velocity_a, hinge_cons->xanchor_a, linear_velocity_b, angular_velocity_b, hinge_cons->xanchor_b);
  float vn = phy_vector2_dot(rv, hinge_cons->normal);
  float lambda = -(hinge_cons->bias + vn) * hinge_cons->mass;
  float limit = hinge_cons->max_impulse;
  float lambda0 = hinge_cons->impulse;
  hinge_cons->impulse = phy_fclamp(lambda0 + lambda, -limit, limit);
  lambda = hinge_cons->impulse - lambda0;
  phy_vector2 impulse = phy_vector2_mul(hinge_cons->normal, lambda);
  if (a)  {
    phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), hinge_cons->xanchor_a);
  }
  if (b) {
    phy_rigid_body_apply_impulse(b, impulse, hinge_cons->xanchor_b);
  }
}

