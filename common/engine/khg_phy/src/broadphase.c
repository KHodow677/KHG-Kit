#include "khg_phy/broadphase.h"
#include "khg_phy/aabb.h"
#include "khg_phy/bvh.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_array.h"
#include <stdbool.h>

static inline bool nvBroadPhase_early_out(phy_space *space, phy_rigid_body *a, phy_rigid_body *b) {
  if (a->id >= b->id) {
    return true;
  }
  if (!a->collision_enabled || !b->collision_enabled) {
    return true;
  }
  if (a->type == PHY_RIGID_BODY_TYPE_STATIC && b->type == PHY_RIGID_BODY_TYPE_STATIC) {
    return true;
  }
  if (a->collision_group == b->collision_group && a->collision_group != 0) {
    return true;
  }
  if ((a->collision_mask & b->collision_category) == 0 || (b->collision_mask & a->collision_category) == 0) {
    return true;
  }
  for (unsigned int i = 0; i < space->constraints->size; i++) {
    phy_constraint *cons = space->constraints->data[i];
    if (cons->ignore_collision && ((a == cons->a && b == cons->b) || (a == cons->b && b == cons->a))) {
      return true;
    }
  }
  return false;
}

void phy_broadphase_brute_force(phy_space *space) {
  phy_memory_pool_clear(space->broadphase_pairs);
  for (unsigned int i = 0; i < space->bodies->size; i++) {
    phy_rigid_body *a = (phy_rigid_body *)space->bodies->data[i];
    phy_transform xform_a = (phy_transform){a->origin, a->angle};
    phy_aabb abox = phy_rigid_body_get_aabb(a);
    for (unsigned int j = 0; j < space->bodies->size; j++) { 
      phy_rigid_body *b = (phy_rigid_body *)space->bodies->data[j];
      if (nvBroadPhase_early_out(space, a, b)) {
        continue;
      }
      phy_broadphase_pair pair = {a, b};
      phy_transform xform_b = (phy_transform){b->origin, b->angle};
      phy_aabb bbox = phy_rigid_body_get_aabb(b);
      bool overlaps = false;
      if (phy_collide_aabb_x_aabb(abox, bbox)) {
        for (unsigned int k = 0; k < a->shapes->size; k++) {
          phy_shape *shape_a = a->shapes->data[k];
          phy_aabb sabox = phy_shape_get_aabb(shape_a, xform_a);
          for (unsigned int l = 0; l < b->shapes->size; l++) {
            phy_shape *shape_b = b->shapes->data[l];
            phy_aabb sbbox = phy_shape_get_aabb(shape_b, xform_b);
            if (phy_collide_aabb_x_aabb(sabox, sbbox)) {
              overlaps = true;
              break;
            }
          }
          if (overlaps) {
            break;
          }
        }
      }
      if (overlaps) {
        phy_memory_pool_add(space->broadphase_pairs, &pair);
      }
    }
  }
}

void phy_broadphase_bvh(phy_space *space) {
  phy_memory_pool_clear(space->broadphase_pairs);
  for (unsigned int i = 0; i < space->bodies->size; i++) {
    phy_rigid_body *body = space->bodies->data[i];
    phy_aabb aabb = phy_rigid_body_get_aabb(body);
    body->bvh_median_x = (aabb.min_x + aabb.max_x) * 0.5;
    body->bvh_median_y = (aabb.min_y + aabb.max_y) * 0.5;
  }
  phy_bvh_node *bvh = phy_bvh_tree_new(space->bodies);
  for (unsigned int i = 0; i < space->bodies->size; i++) {
    phy_rigid_body *a = space->bodies->data[i];
    phy_aabb aabb = phy_rigid_body_get_aabb(a);
    phy_array_clear(space->bvh_traversed, NULL);
    phy_bvh_node_collide(bvh, aabb, space->bvh_traversed);
    if (space->bvh_traversed->size == 0) {
      continue;
    }
    for (unsigned int j = 0; j < space->bvh_traversed->size; j++) {
      phy_rigid_body *b = space->bvh_traversed->data[j];
      if (nvBroadPhase_early_out(space, a, b)) {
        continue;
      }
      phy_aabb bbox = phy_rigid_body_get_aabb(b);
      phy_broadphase_pair pair = {a, b};
      if (phy_collide_aabb_x_aabb(aabb, bbox)) {
        phy_memory_pool_add(space->broadphase_pairs, &pair);
      }
    }
  }
  phy_bvh_tree_free(bvh);
}

void phy_broadphase_finalize(phy_space *space) {
  phy_hashmap_clear(space->removed_contacts);
  void *map_val;
  unsigned int map_iter = 0;
  while (phy_hashmap_iter(space->contacts, &map_iter, &map_val)) {
    phy_persistent_contact_pair *pcp = map_val;
    phy_rigid_body *a = pcp->body_a;
    phy_rigid_body *b = pcp->body_b;
    phy_aabb abox = phy_rigid_body_get_aabb(a);
    phy_aabb bbox = phy_rigid_body_get_aabb(b);
    if (!phy_collide_aabb_x_aabb(abox, bbox)) {
      for (unsigned int k = 0; k < a->shapes->size; k++) {
        phy_shape *shape_a = a->shapes->data[k];
        for (unsigned int l = 0; l < b->shapes->size; l++) {
          phy_shape *shape_b = b->shapes->data[l];
          phy_persistent_contact_pair *key = &(phy_persistent_contact_pair){.shape_a=shape_a, .shape_b=shape_b};
          phy_persistent_contact_pair *pcp = phy_hashmap_get(space->contacts, key);
          if (pcp) {
            for (unsigned int c = 0; c < pcp->contact_count; c++) {
              phy_contact *contact = &pcp->contacts[c];
              phy_contact_event event = { .body_a = pcp->body_a, .body_b = pcp->body_b, .shape_a = pcp->shape_a, .shape_b = pcp->shape_b, .normal = pcp->normal, .penetration = contact->separation, .position = phy_vector2_add(pcp->body_a->position, contact->anchor_a), .normal_impulse = {contact->solver_info.normal_impulse}, .friction_impulse = {contact->solver_info.tangent_impulse}, .id = contact->id };
              if (space->listener && !contact->remove_invoked) {
                if (space->listener->on_contact_removed) {
                  space->listener->on_contact_removed(space, event, space->listener_arg);
                }
                contact->remove_invoked = true;
              }
            }
            phy_hashmap_set(space->removed_contacts, pcp);
          }
        }
      }
    }
  }
  map_val = NULL;
  map_iter = 0;
  while (phy_hashmap_iter(space->removed_contacts, &map_iter, &map_val)) {
    phy_persistent_contact_pair *pcp = map_val;
    phy_hashmap_remove(space->contacts, pcp);
  }
}

