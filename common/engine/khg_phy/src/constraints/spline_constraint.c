#include "khg_phy/constraints/spline_constraint.h"
#include "khg_phy/space.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <math.h>

phy_constraint *phy_spline_constraint_new(phy_spline_constraint_initializer init) {
  phy_constraint *cons = malloc(sizeof(phy_constraint));
  if (!cons) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }

  if (!init.body) {
    utl_error_func("Both bodies are null", utl_user_defined_data);
    free(cons);
    return NULL;
  }
  cons->a = init.body;
  cons->b = NULL;
  cons->type = PHY_CONSTRAINT_TYPE_SPLINE;
  cons->ignore_collision = false;
  cons->def = (phy_spline_constraint *)malloc(sizeof(phy_spline_constraint));
  if (!cons->def) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
    free(cons);
    return NULL; 
  }
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  spline_cons->anchor_a = phy_vector2_sub(init.anchor, init.body->position);
  spline_cons->anchor_b = init.anchor;
  spline_cons->max_force = init.max_force;
  spline_cons->xanchor_a = phy_vector2_zero;
  spline_cons->xanchor_b = phy_vector2_zero;
  spline_cons->normal = phy_vector2_zero;
  spline_cons->bias = 0.0;
  spline_cons->mass = 0.0;
  spline_cons->impulse = 0.0;
  spline_cons->max_impulse = 0.0;
  return cons;
}

phy_rigid_body *phy_spline_constraint_get_body(const phy_constraint *cons) {
  return cons->a;
}

void phy_spline_constraint_set_anchor(phy_constraint *cons, phy_vector2 anchor) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  spline_cons->anchor = anchor;
  spline_cons->anchor_a = phy_vector2_sub(spline_cons->anchor, cons->a->position);
  spline_cons->anchor_b = spline_cons->anchor;
}

phy_vector2 phy_spline_constraint_get_anchor(const phy_constraint *cons) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  return spline_cons->anchor;
}

void phy_spline_constraint_set_max_force(phy_constraint *cons, float max_force) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  spline_cons->max_force = max_force;
}

float phy_spline_constraint_get_max_force(const phy_constraint *cons) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  return spline_cons->max_force;
}

int phy_spline_constraint_set_control_points(phy_constraint *cons, phy_vector2 *points, unsigned int num_points) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  if (num_points < 4) {
    utl_error_func("Spline path has less than 4 control points", utl_user_defined_data);
    return 1;
  }
  spline_cons->num_controls = num_points;
  for (unsigned int i = 0; i < num_points; i++) {
    spline_cons->controls[i] = points[i];
  }
  return 0;
}

phy_vector2 *phy_spline_constraint_get_control_points(const phy_constraint *cons) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  return spline_cons->controls;
}

unsigned int phy_spline_constraint_get_number_of_control_points(const phy_constraint *cons) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  return spline_cons->num_controls;
}

static inline phy_vector2 catmull_rom(phy_vector2 p0, phy_vector2 p1, phy_vector2 p2, phy_vector2 p3, float t) {
  float t2 = t * t;
  float t3 = t2 * t;
  float x = 0.5 * ((2.0 * p1.x) + (-p0.x + p2.x) * t + (2.0 * p0.x - 5.0 * p1.x + 4.0 * p2.x - p3.x) * t2 + (-p0.x + 3.0 * p1.x - 3.0 * p2.x + p3.x) * t3);
  float y = 0.5 * ((2 * p1.y) + (-p0.y + p2.y) * t + (2.0 * p0.y - 5.0 * p1.y + 4.0 * p2.y - p3.y) * t2 + (-p0.y + 3.0 * p1.y - 3.0 * p2.y + p3.y) * t3);
  return phy_vector2_new(x, y);
}

static inline float gss_for_t(phy_vector2 p0, phy_vector2 p1, phy_vector2 p2, phy_vector2 p3, phy_vector2 p, float tolerance) {
  float a = 0.0;
  float b = 1.0;
  float t1 = b - (b - a) * PHY_INV_PHI;
  float t2 = a + (b - a) * PHY_INV_PHI;
  while (fabs(b - a) > tolerance) {
    phy_vector2 v1 = catmull_rom(p0, p1, p2, p3, t1);
    phy_vector2 v2 = catmull_rom(p0, p1, p2, p3, t2);
    if (phy_vector2_dist2(v1, p) < phy_vector2_dist2(v2, p)) {
      b = t2;
    } 
    else {
      a = t1;
    }
    t1 = b - (b - a) * PHY_INV_PHI;
    t2 = a + (b - a) * PHY_INV_PHI;
  }
  return (a + b) / 2.0;
}

static phy_vector2 spline_closest(phy_spline_constraint *spline, phy_vector2 point) {
  phy_vector2 *controls = spline->controls;
  unsigned int num_controls = spline->num_controls;
  unsigned int num_segments = num_controls - 3;
  unsigned int sample_per_segment = PHY_SPLINE_CONSTRAINT_SAMPLES / num_segments;
  phy_vector2 segment0 = phy_vector2_zero;
  phy_vector2 segment1 = phy_vector2_zero;
  phy_vector2 segment2 = phy_vector2_zero;
  phy_vector2 segment3 = phy_vector2_zero;
  float min_dist = INFINITY;
  for (unsigned int i = 0; i < num_segments; i++) {
    for (unsigned int j = 0; j < sample_per_segment; j++) {
      float t = (float)j / (float)(sample_per_segment - 1);
      phy_vector2 p0 = controls[i];
      phy_vector2 p1 = controls[i + 1];
      phy_vector2 p2 = controls[i + 2];
      phy_vector2 p3 = controls[i + 3];
      phy_vector2 p = catmull_rom(p0, p1, p2, p3, t);
      float dist = phy_vector2_dist2(p, point);
      if (dist < min_dist) {
        min_dist = dist;
        segment0 = p0;
        segment1 = p1;
        segment2 = p2;
        segment3 = p3;
      }
    }
  }
  float t = gss_for_t(segment0, segment1, segment2, segment3, point, PHY_SPLINE_CONSTRAINT_TOLERANCE);
  return catmull_rom(segment0, segment1, segment2, segment3, t);
}

void phy_spline_constraint_presolve(phy_space *space, phy_constraint *cons, float dt, float inv_dt) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  phy_rigid_body *a = cons->a;
  phy_vector2 rpa, rpb;
  float invmass_a, invmass_b, invinertia_a, invinertia_b;
  spline_cons->xanchor_a = phy_vector2_rotate(spline_cons->anchor_a, a->angle);
  rpa = phy_vector2_add(spline_cons->xanchor_a, a->position);
  invmass_a = a->invmass;
  invinertia_a = a->invinertia;
  phy_vector2 spline_point = spline_closest(spline_cons, rpa);
  spline_cons->xanchor_b = phy_vector2_zero;
  rpb = spline_point;
  invmass_b = invinertia_b = 0.0;
  phy_vector2 delta = phy_vector2_sub(rpb, rpa);
  if (phy_vector2_len2(delta) == 0.0) spline_cons->normal = phy_vector2_zero;
  else spline_cons->normal = phy_vector2_normalize(delta);
  float offset = phy_vector2_len(delta);
  spline_cons->bias = space->settings.baumgarte * inv_dt * offset;
  spline_cons->mass = 1.0 / phy_calc_mass_k(spline_cons->normal, spline_cons->xanchor_a, spline_cons->xanchor_b, invmass_a, invmass_b, invinertia_a, invinertia_b);
  spline_cons->max_impulse = spline_cons->max_force * dt;
}

void phy_spline_constraint_warmstart(phy_space *space, phy_constraint *cons) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  if (space->settings.warmstarting) {
    phy_vector2 impulse = phy_vector2_mul(spline_cons->normal, spline_cons->impulse);
    phy_rigid_body_apply_impulse(cons->a, phy_vector2_neg(impulse), spline_cons->xanchor_a);
  }
  else {
    spline_cons->impulse = 0.0;
  }
}

void phy_spline_constraint_solve(phy_constraint *cons) {
  phy_spline_constraint *spline_cons = (phy_spline_constraint *)cons->def;
  phy_rigid_body *a = cons->a;
  if (phy_vector2_is_zero(spline_cons->normal)) {
    return;
  }
  phy_vector2 linear_velocity_a, linear_velocity_b;
  float angular_velocity_a, angular_velocity_b;
  linear_velocity_a = a->linear_velocity;
  angular_velocity_a = a->angular_velocity;
  linear_velocity_b = phy_vector2_zero;
  angular_velocity_b = 0.0;
  phy_vector2 rv = phy_calc_relative_velocity(linear_velocity_a, angular_velocity_a, spline_cons->xanchor_a, linear_velocity_b, angular_velocity_b, spline_cons->xanchor_b);
  float vn = phy_vector2_dot(rv, spline_cons->normal);
  float lambda = -(spline_cons->bias + vn) * spline_cons->mass;
  float limit = spline_cons->max_impulse;
  float lambda0 = spline_cons->impulse;
  spline_cons->impulse = phy_fclamp(lambda0 + lambda, -limit, limit);
  lambda = spline_cons->impulse - lambda0;
  phy_vector2 impulse = phy_vector2_mul(spline_cons->normal, lambda);
  phy_rigid_body_apply_impulse(a, phy_vector2_neg(impulse), spline_cons->xanchor_a);
}

