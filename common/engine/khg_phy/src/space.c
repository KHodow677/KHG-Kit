#include "khg_phy/space.h"
#include "khg_phy/body.h"
#include "khg_phy/collision.h"
#include "khg_phy/contact.h"
#include "khg_phy/math.h"
#include "khg_phy/narrowphase.h"
#include "khg_phy/constraints/contact_constraint.h"
#include "khg_phy/core/phy_array.h"
#include "khg_phy/core/phy_constants.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/error_func.h"
#include <stdint.h>
#include <stdlib.h>

#define phy_iter_bodies(iter) for (size_t iter = 0; iter < space->bodies->size; iter++)

phy_space *phy_space_new() {
  phy_space *space = malloc(sizeof(phy_space));
  if (!space) {
    return NULL;
  }
  space->bodies = phy_array_new();
  space->constraints = phy_array_new();
  phy_space_set_gravity(space, phy_vector2_new(0.0, PHY_GRAV_EARTH));
  space->settings = (phy_space_settings){ .baumgarte = 0.2, .penetration_slop = 0.05, .contact_position_correction = PHY_CONTACT_POSITION_CORRECTION_BAUMGARTE, .velocity_iterations = 8, .position_iterations = 4, .substeps = 1, .linear_damping = 0.0005, .angular_damping = 0.0005, .warmstarting = true, .restitution_mix = PHY_COEFFICIENT_MIX_SQRT, .friction_mix = PHY_COEFFICIENT_MIX_SQRT };
  phy_space_set_broadphase(space, PHY_BROADPHASE_ALGORITHM_BRUTE_FORCE);
  space->broadphase_pairs = phy_memory_pool_new(sizeof(phy_broadphase_pair), PHY_BPH_POOL_INITIAL_SIZE);
  space->contacts = phy_hashmap_new(sizeof(phy_persistent_contact_pair), 0, phy_persistent_contact_pair_hash);
  space->removed_contacts = phy_hashmap_new(sizeof(phy_persistent_contact_pair), 0, phy_persistent_contact_pair_hash);
  space->bvh_traversed = phy_array_new();
  space->listener = NULL;
  space->listener_arg = NULL;
  space->id_counter = 1;
  return space;
}

void phy_space_free(phy_space *space) {
  if (!space) {
    return;
  }
  phy_space_clear(space, true);
  phy_array_free(space->bodies);
  phy_array_free(space->constraints);
  phy_memory_pool_free(space->broadphase_pairs);
  phy_hashmap_free(space->contacts);
  phy_hashmap_free(space->removed_contacts);
  phy_array_free(space->bvh_traversed);
  free(space->listener);
  free(space);
}

void phy_space_set_gravity(phy_space *space, phy_vector2 gravity) {
  space->gravity = gravity;
}

phy_vector2 phy_space_get_gravity(const phy_space *space) {
  return space->gravity;
}

void phy_space_set_broadphase(phy_space *space, phy_broadphase_algorithm broadphase_alg_type) {
  switch (broadphase_alg_type) {
    case PHY_BROADPHASE_ALGORITHM_BRUTE_FORCE:
      space->broadphase_algorithm = PHY_BROADPHASE_ALGORITHM_BRUTE_FORCE;
      return;
    case PHY_BROADPHASE_ALGORITHM_BVH:
      space->broadphase_algorithm = PHY_BROADPHASE_ALGORITHM_BVH;
      return;
  }
}

phy_broadphase_algorithm phy_space_get_broadphase(const phy_space *space) {
  return space->broadphase_algorithm;
}

phy_space_settings *phy_space_get_settings(phy_space *space) {
  return &space->settings;
}

int phy_space_set_contact_listener(phy_space *space, phy_contact_listener listener, void *user_arg) {
  space->listener = (phy_contact_listener *)malloc(sizeof(phy_contact_listener));
  if (!space->listener) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  *space->listener = listener;
  space->listener_arg = user_arg;
  return 0;
}

phy_contact_listener *phy_space_get_contact_listener(const phy_space *space) {
  return space->listener;
}

int phy_space_clear(phy_space *space, bool free_all) {
  if (free_all) {
    if (phy_array_clear(space->bodies, (void (*)(void *))phy_rigid_body_free)) {
      return 1;
    }
    if (phy_array_clear(space->constraints, (void (*)(void *))phy_constraint_free)) {
      return 1;
    }
    phy_memory_pool_clear(space->broadphase_pairs);
    phy_hashmap_clear(space->contacts);
  }
  else {
    if (phy_array_clear(space->bodies, NULL)) {
      return 1;
    }
    if (phy_array_clear(space->constraints, NULL)) {
      return 1;
    }
    phy_memory_pool_clear(space->broadphase_pairs);
    phy_hashmap_clear(space->contacts);
  }
  return 0;
}

int phy_space_add_rigidbody(phy_space *space, phy_rigid_body *body) {
  if (body->space == space) {
    utl_error_func("Body already exists in space", utl_user_defined_data);
    return 2;
  }
  if (phy_array_add(space->bodies, body)) {
    return 1;
  }
  body->space = space;
  body->id = space->id_counter++;
  return 0;
}

int phy_space_remove_rigidbody(phy_space *space, phy_rigid_body *body) {
  if (phy_array_remove(space->bodies, body) == (size_t)(-1)) {
    return 1;
  }
  for (size_t i = 0; i < space->broadphase_pairs->current_size; i++) {
    void *pool_i = (char *)space->broadphase_pairs->pool + i * space->broadphase_pairs->chunk_size;
    phy_broadphase_pair *pair = (phy_broadphase_pair *)pool_i;
    phy_rigid_body *body_a = pair->a;
    phy_rigid_body *body_b = pair->b;
    if (body_a == body || body_b == body) {
      pair->a = NULL;
      pair->b = NULL;
    }
  }
  void *map_val;
  size_t map_iter = 0;
  while (phy_hashmap_iter(body->space->contacts, &map_iter, &map_val)) {
    phy_persistent_contact_pair *pcp = map_val;
    if (pcp->body_a == body || pcp->body_b == body) {
      phy_persistent_contact_pair_remove(body->space, pcp);
      map_iter = 0;
      continue;
    }
  }
  phy_array *removed_constraints = phy_array_new();
  for (size_t i = 0; i < space->constraints->size; i++) {
    phy_constraint *cons = space->constraints->data[i];
    if (cons->a == body || cons->b == body) {
      phy_array_add(removed_constraints, cons);
    }
  }
  for (size_t i = 0; i < removed_constraints->size; i++) {
    phy_array_remove(space->constraints, removed_constraints->data[i]);
  }
  phy_array_free(removed_constraints);
  return 0;
}

int phy_space_add_constraint(phy_space *space, phy_constraint *cons) {
  for (size_t i = 0; i < space->constraints->size; i++) {
    phy_constraint *lcons = space->constraints->data[i];
    if (lcons == cons) { 
      utl_error_func("Constraint already exists in space", utl_user_defined_data);
      return 2;
    }
  }
  return phy_array_add(space->constraints, cons);
}

int phy_space_remove_constraint(phy_space *space, phy_constraint *cons) {
  if (phy_array_remove(space->constraints, cons) == (size_t)(-1)) {
    return 1;
  }
  return 0;
}

bool phy_space_iter_bodies(phy_space *space, phy_rigid_body **body, size_t *index) {
  *body = space->bodies->data[(*index)++];
  return (*index <= space->bodies->size);
}

bool phy_space_iter_constraints(phy_space *space, phy_constraint **cons, size_t *index) {
  *cons = space->constraints->data[(*index)++];
  return (*index <= space->constraints->size);
}

void phy_space_step(phy_space *space, float dt) {
  if (dt == 0.0 || space->settings.substeps <= 0) {
    return;
  }
  uint32_t substeps = space->settings.substeps;
  uint32_t velocity_iters = space->settings.velocity_iterations;
  size_t l;
  void *map_val;
  dt /= (float)substeps;
  float inv_dt = 1.0 / dt;
  for (uint32_t substep = 0; substep < substeps; substep++) {
    phy_iter_bodies(body_i) {
      phy_rigid_body *body = (phy_rigid_body *)space->bodies->data[body_i];
      phy_rigid_body_integrate_accelerations(body, space->gravity, dt);
    }
    switch (space->broadphase_algorithm) {
      case PHY_BROADPHASE_ALGORITHM_BRUTE_FORCE:
        phy_broadphase_brute_force(space);
        break;
      case PHY_BROADPHASE_ALGORITHM_BVH:
        phy_broadphase_bvh(space);
        break;
    }
    phy_broadphase_finalize(space);
    phy_narrow_phase(space);
    for (size_t i = 0; i < space->constraints->size; i++) {
      phy_constraint_presolve(space, (phy_constraint *)space->constraints->data[i], dt, inv_dt);
    }
    l = 0;
    while (phy_hashmap_iter(space->contacts, &l, &map_val)) {
      phy_persistent_contact_pair *pcp = map_val;
      phy_contact_presolve(space, pcp, inv_dt);
    }
    for (size_t i = 0; i < space->constraints->size; i++) {
      phy_constraint_warmstart(space, (phy_constraint *)space->constraints->data[i]);
    }
    l = 0;
    while (phy_hashmap_iter(space->contacts, &l, &map_val)) {
      phy_persistent_contact_pair *pcp = map_val;
      phy_contact_warmstart(space, pcp);
    }
    for (size_t i = 0; i < velocity_iters; i++) {
      for (size_t j = 0; j < space->constraints->size; j++) {
        phy_constraint_solve((phy_constraint *)space->constraints->data[j], inv_dt);
      }
      l = 0;
      while (phy_hashmap_iter(space->contacts, &l, &map_val)) {
        phy_persistent_contact_pair *pcp = map_val;
        phy_contact_solve_velocity(pcp);
      }
    }
    phy_iter_bodies(body_i) {
      phy_rigid_body *body = (phy_rigid_body *)space->bodies->data[body_i];
      phy_rigid_body_integrate_velocities(body, dt);
      body->origin = phy_vector2_sub(body->position, phy_vector2_rotate(body->com, body->angle));
      if (body->type != PHY_RIGID_BODY_TYPE_STATIC) {
        body->cache_aabb = false;
        body->cache_transform = false;
      }
    }
  }
}

void phy_space_cast_ray(phy_space *space, phy_vector2 from, phy_vector2 to, phy_raycast_result *results_array, size_t *num_hits, size_t capacity) {
  *num_hits = 0;
  phy_vector2 delta = phy_vector2_sub(to, from);
  phy_vector2 dir = phy_vector2_normalize(delta);
  float maxsq = phy_vector2_len2(delta);
  phy_iter_bodies(body_i) {
    phy_rigid_body *body = space->bodies->data[body_i];
    phy_transform xform = {body->origin, body->angle};
    phy_raycast_result closest_result;
    float min_dist = INFINITY;
    float any_hit = false;
    for (size_t shape_i = 0; shape_i < body->shapes->size; shape_i++) {
      phy_shape *shape = body->shapes->data[shape_i];
      phy_raycast_result result;
      bool hit = false;
      switch (shape->type) {
        case PHY_SHAPE_TYPE_CIRCLE:
          hit = phy_collide_ray_x_circle(&result, from, dir, maxsq, shape, xform);
          break;
        case PHY_SHAPE_TYPE_POLYGON:
          hit = phy_collide_ray_x_polygon(&result, from, dir, maxsq, shape, xform);
          break;
      }
      if (hit) {
        any_hit = true;
        float dist = phy_vector2_dist2(from, result.position);
        if (dist < min_dist) {
          min_dist = dist;
          closest_result = result;
        }
      }
    }
    if (any_hit) {
      closest_result.body = body;
      results_array[(*num_hits)++] = closest_result;
      if ((*num_hits) == capacity) {
        break;
      }
    }
  }
}

