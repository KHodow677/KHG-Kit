/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/broadphase.h"
#include "khg_phy/core/phy_array.h"
#include "khg_phy/aabb.h"
#include "khg_phy/space.h"
#include "khg_phy/bvh.h"


/**
 * @file broadphase.c
 * 
 * @brief Broad-phase algorithms.
 */


/**
 * @brief Early-out from checking collisions.
 */
static inline nv_bool nvBroadPhase_early_out(
    phy_space *space,
    phy_rigid_body *a,
    phy_rigid_body *b
) {
    // Same body or already checked
    if (a->id >= b->id)
        return true;

    // One of the bodies have collision detection disabled
    if (!a->collision_enabled || !b->collision_enabled)
        return true;

    // Two static bodies do not need to interact
    if (a->type == PHY_RIGID_BODY_TYPE_STATIC && b->type == PHY_RIGID_BODY_TYPE_STATIC)
        return true;

    // Bodies share the same non-zero group
    if (a->collision_group == b->collision_group && a->collision_group != 0)
        return true;

    // One of the collision mask doesn't fit the category
    if ((a->collision_mask & b->collision_category) == 0 ||
        (b->collision_mask & a->collision_category) == 0)
        return true;

    // TODO: There must be a more efficient way
    for (size_t i = 0; i < space->constraints->size; i++) {
        phy_constraint *cons = space->constraints->data[i];

        if (
            cons->ignore_collision &&
            ((a == cons->a && b == cons->b) || (a == cons->b && b == cons->a))
        ) {
            return true;
        }
    }

    return false;
}


void nv_broadphase_brute_force(phy_space *space) {
    NV_TRACY_ZONE_START;

    phy_memory_pool_clear(space->broadphase_pairs);

    for (size_t i = 0; i < space->bodies->size; i++) {
        phy_rigid_body *a = (phy_rigid_body *)space->bodies->data[i];
        nvTransform xform_a = (nvTransform){a->origin, a->angle};
        phy_aabb abox = nvRigidBody_get_aabb(a);

        for (size_t j = 0; j < space->bodies->size; j++) {
            phy_rigid_body *b = (phy_rigid_body *)space->bodies->data[j];

            if (nvBroadPhase_early_out(space, a, b)) continue;

            nvBroadPhasePair pair = {a, b};

            nvTransform xform_b = (nvTransform){b->origin, b->angle};
            phy_aabb bbox = nvRigidBody_get_aabb(b);

            // First check the body AABB, then check every shape AABB
            // TODO: Improve this & use in BVH as well
            nv_bool overlaps = false;
            if (nv_collide_aabb_x_aabb(abox, bbox)) {
                for (size_t k = 0; k < a->shapes->size; k++) {
                    nvShape *shape_a = a->shapes->data[k];
                    phy_aabb sabox = nvShape_get_aabb(shape_a, xform_a);

                    for (size_t l = 0; l < b->shapes->size; l++) {
                        nvShape *shape_b = b->shapes->data[l];
                        phy_aabb sbbox = nvShape_get_aabb(shape_b, xform_b);

                        if (nv_collide_aabb_x_aabb(sabox, sbbox)) {
                            overlaps = true;
                            break;
                        }
                    }

                    if (overlaps)
                        break;
                }
            }

            if (overlaps) {
                phy_memory_pool_add(space->broadphase_pairs, &pair);
            }
        }
    }

    NV_TRACY_ZONE_END;
}


void nv_broadphase_BVH(phy_space *space) {
    NV_TRACY_ZONE_START;

    phy_memory_pool_clear(space->broadphase_pairs);

    nvPrecisionTimer timer;
    NV_PROFILER_START(timer);
    // Prepare median splitting coords
    for (size_t i = 0; i < space->bodies->size; i++) {
        phy_rigid_body *body = space->bodies->data[i];
        phy_aabb aabb = nvRigidBody_get_aabb(body);
        body->bvh_median_x = (aabb.min_x + aabb.max_x) * 0.5;
        body->bvh_median_y = (aabb.min_y + aabb.max_y) * 0.5;
    }

    // Build the tree top-down
    nvBVHNode *bvh = nvBVHTree_new(space->bodies);
    NV_PROFILER_STOP(timer, space->profiler.bvh_build);

    NV_PROFILER_START(timer);
    for (size_t i = 0; i < space->bodies->size; i++) {
        phy_rigid_body *a = space->bodies->data[i];
        phy_aabb aabb = nvRigidBody_get_aabb(a);

        phy_array_clear(space->bvh_traversed, NULL);
        nvBVHNode_collide(bvh, aabb, space->bvh_traversed);
        if (space->bvh_traversed->size == 0) continue;

        for (size_t j = 0; j < space->bvh_traversed->size; j++) {
            phy_rigid_body *b = space->bvh_traversed->data[j];

            if (nvBroadPhase_early_out(space, a, b)) continue;

            phy_aabb bbox = nvRigidBody_get_aabb(b);

            nvBroadPhasePair pair = {a, b};

            if (nv_collide_aabb_x_aabb(aabb, bbox)) {
                phy_memory_pool_add(space->broadphase_pairs, &pair);
            }
        }
    }
    NV_PROFILER_STOP(timer, space->profiler.bvh_traverse);

    NV_PROFILER_START(timer);
    nvBVHTree_free(bvh);
    NV_PROFILER_STOP(timer, space->profiler.bvh_free);

    NV_TRACY_ZONE_END;
}

void nv_broadphase_finalize(phy_space *space) {
    NV_TRACY_ZONE_START;

    /*
        Keeping the removed contacts in the main iteration then actually removing
        them in another iteration is way more performant than modifying the map
        in single iteration. Resetting the iterator causes very bad performance spikes
        in large scenes.
    */

    phy_hashmap_clear(space->removed_contacts);

    void *map_val;
    size_t map_iter = 0;
    while (phy_hashmap_iter(space->contacts, &map_iter, &map_val)) {
        phy_persistent_contact_pair *pcp = map_val;

        phy_rigid_body *a = pcp->body_a;
        phy_rigid_body *b = pcp->body_b;
        phy_aabb abox = nvRigidBody_get_aabb(a);
        phy_aabb bbox = nvRigidBody_get_aabb(b);

        if (!nv_collide_aabb_x_aabb(abox, bbox)) {
            for (size_t k = 0; k < a->shapes->size; k++) {
                nvShape *shape_a = a->shapes->data[k];

                for (size_t l = 0; l < b->shapes->size; l++) {
                    nvShape *shape_b = b->shapes->data[l];

                    phy_persistent_contact_pair *key = &(phy_persistent_contact_pair){.shape_a=shape_a, .shape_b=shape_b};

                    phy_persistent_contact_pair *pcp = phy_hashmap_get(space->contacts, key);
                    if (pcp) {
                        for (size_t c = 0; c < pcp->contact_count; c++) {
                            phy_contact *contact = &pcp->contacts[c];

                            nvContactEvent event = {
                                .body_a = pcp->body_a,
                                .body_b = pcp->body_b,
                                .shape_a = pcp->shape_a,
                                .shape_b = pcp->shape_b,
                                .normal = pcp->normal,
                                .penetration = contact->separation,
                                .position = nvVector2_add(pcp->body_a->position, contact->anchor_a),
                                .normal_impulse = {contact->solver_info.normal_impulse},
                                .friction_impulse = {contact->solver_info.tangent_impulse},
                                .id = contact->id
                            };

                            if (space->listener && !contact->remove_invoked) {
                                if (space->listener->on_contact_removed)
                                    space->listener->on_contact_removed(space, event, space->listener_arg);
                                contact->remove_invoked = true;
                            };
                        }

                        phy_hashmap_set(space->removed_contacts, pcp);
                    }
                }
            }
        }
    }

    // Actually remove all "removed" contacts
    map_val = NULL;
    map_iter = 0;
    while (phy_hashmap_iter(space->removed_contacts, &map_iter, &map_val)) {
        phy_persistent_contact_pair *pcp = map_val;

        phy_hashmap_remove(space->contacts, pcp);
    }

    NV_TRACY_ZONE_END;
}
