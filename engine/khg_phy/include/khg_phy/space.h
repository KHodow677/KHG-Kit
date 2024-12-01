#pragma once

#include "khg_phy/body.h"
#include "khg_phy/broadphase.h"
#include "khg_phy/collision.h"
#include "khg_phy/contact.h"
#include "khg_phy/space_settings.h"
#include "khg_phy/constraints/constraint.h"
#include "khg_phy/core/phy_array.h"
#include "khg_phy/core/phy_hashmap.h"
#include "khg_phy/core/phy_pool.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct phy_space {
  phy_array *bodies;
  phy_array *constraints;
  phy_hashmap *contacts;
  phy_hashmap *removed_contacts;
  phy_memory_pool *broadphase_pairs;
  phy_array *bvh_traversed;
  uint32_t id_counter;
  phy_vector2 gravity;
  phy_space_settings settings;
  phy_broadphase_algorithm broadphase_algorithm;
  phy_contact_listener *listener;
  void *listener_arg;
} phy_space;

phy_space *phy_space_new();
void phy_space_free(phy_space *space);

void phy_space_set_gravity(phy_space *space, phy_vector2 gravity);
phy_vector2 phy_space_get_gravity(const phy_space *space);

void phy_space_set_broadphase(phy_space *space, phy_broadphase_algorithm broadphase_alg_type);
phy_broadphase_algorithm phy_space_get_broadphase(const phy_space *space);

phy_space_settings *phy_space_get_settings(phy_space *space);

int phy_space_set_contact_listener(phy_space *space, phy_contact_listener listener, void *user_arg);
phy_contact_listener *phy_space_get_contact_listener(const phy_space *space);

int phy_space_clear(phy_space *space, bool free_all);

int phy_space_add_rigidbody(phy_space *space, phy_rigid_body *body);
int phy_space_remove_rigidbody(phy_space *space, phy_rigid_body *body);

int phy_space_add_constraint(phy_space *space, phy_constraint *cons);
int phy_space_remove_constraint(phy_space *space, phy_constraint *cons);

bool phy_space_iter_bodies(phy_space *space, phy_rigid_body **body, size_t *index);
bool phy_space_iter_constraints(phy_space *space, phy_constraint **cons, size_t *index);

void phy_space_step(phy_space *space, float dt);

void phy_space_cast_ray(phy_space *space, phy_vector2 from, phy_vector2 to, phy_raycast_result *results_array, size_t *num_hits, size_t capacity);

