/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/space.h"
#include "khg_phy/constants.h"
#include "khg_phy/body.h"
#include "khg_phy/collision.h"
#include "khg_phy/contact.h"
#include "khg_phy/core/phy_array.h"
#include "khg_phy/math.h"
#include "khg_phy/narrowphase.h"
#include "khg_phy/constraints/contact_constraint.h"
#include "khg_utl/error_func.h"


/**
 * @file space.c
 * 
 * @brief Space struct and its methods.
 */


#define ITER_BODIES(iter) for (size_t iter = 0; iter < space->bodies->size; iter++)


phy_space *nvSpace_new() {
    phy_space *space = NV_NEW(phy_space);
    if (!space) return NULL;

    space->bodies = phy_array_new();
    space->constraints = phy_array_new();

    nvSpace_set_gravity(space, NV_VECTOR2(0.0, NV_GRAV_EARTH));

    space->settings = (nvSpaceSettings){
        .baumgarte = 0.2,
        .penetration_slop = 0.05,
        .contact_position_correction = PHY_CONTACT_POSITION_CORRECTION_BAUMGARTE,
        .velocity_iterations = 8,
        .position_iterations = 4,
        .substeps = 1,
        .linear_damping = 0.0005,
        .angular_damping = 0.0005,
        .warmstarting = true,
        .restitution_mix = PHY_COEFFICIENT_MIX_SQRT,
        .friction_mix = PHY_COEFFICIENT_MIX_SQRT
    };

    nvSpace_set_broadphase(space, nvBroadPhaseAlg_BRUTE_FORCE);

    space->broadphase_pairs = phy_memory_pool_new(sizeof(nvBroadPhasePair), NV_BPH_POOL_INITIAL_SIZE);
    space->contacts = phy_hashmap_new(sizeof(phy_persistent_contact_pair), 0, nvPersistentContactPair_hash);
    space->removed_contacts = phy_hashmap_new(sizeof(phy_persistent_contact_pair), 0, nvPersistentContactPair_hash);
    space->bvh_traversed = phy_array_new();

    space->listener = NULL;
    space->listener_arg = NULL;

    nvProfiler_reset(&space->profiler);

    space->id_counter = 1;

    return space;
}

void nvSpace_free(phy_space *space) {
    if (!space) return;

    nvSpace_clear(space, true);
    phy_array_free(space->bodies);
    phy_array_free(space->constraints);
    phy_memory_pool_free(space->broadphase_pairs);
    phy_hashmap_free(space->contacts);
    phy_hashmap_free(space->removed_contacts);
    phy_array_free(space->bvh_traversed);
    
    NV_FREE(space->listener);

    NV_FREE(space);
}

void nvSpace_set_gravity(phy_space *space, phy_vector2 gravity) {
    space->gravity = gravity;
}

phy_vector2 nvSpace_get_gravity(const phy_space *space) {
    return space->gravity;
}

void nvSpace_set_broadphase(phy_space *space, nvBroadPhaseAlg broadphase_alg_type) {
    switch (broadphase_alg_type) {
        case nvBroadPhaseAlg_BRUTE_FORCE:
            space->broadphase_algorithm = nvBroadPhaseAlg_BRUTE_FORCE;
            return;

        case nvBroadPhaseAlg_BVH:
            space->broadphase_algorithm = nvBroadPhaseAlg_BVH;
            return;
    }
}

nvBroadPhaseAlg nvSpace_get_broadphase(const phy_space *space) {
    return space->broadphase_algorithm;
}

nvSpaceSettings *nvSpace_get_settings(phy_space *space) {
    return &space->settings;
}

nvProfiler nvSpace_get_profiler(const phy_space *space) {
    return space->profiler;
}

int nvSpace_set_contact_listener(
    phy_space *space,
    nvContactListener listener,
    void *user_arg
) {
    space->listener = NV_NEW(nvContactListener);
    if (!space->listener) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }
    *space->listener = listener;
    space->listener_arg = user_arg;
    return 0;
}

nvContactListener *nvSpace_get_contact_listener(const phy_space *space) {
    return space->listener;
}

int nvSpace_clear(phy_space *space, nv_bool free_all) {
    if (free_all) {
        if (phy_array_clear(space->bodies, (void (*)(void *))nvRigidBody_free)) return 1;
        if (phy_array_clear(space->constraints, (void (*)(void *))phy_constraint_free)) return 1;
        phy_memory_pool_clear(space->broadphase_pairs);
        phy_hashmap_clear(space->contacts);
    }
    else {
        if (phy_array_clear(space->bodies, NULL)) return 1;
        if (phy_array_clear(space->constraints, NULL)) return 1;
        phy_memory_pool_clear(space->broadphase_pairs);
        phy_hashmap_clear(space->contacts);
    }
    return 0;
}

int nvSpace_add_rigidbody(phy_space *space, phy_rigid_body *body) {
    if (body->space == space) {
        utl_error_func("Body already exists in space", utl_user_defined_data);
        return 2;
    }

    if (phy_array_add(space->bodies, body))
        return 1;

    body->space = space;
    body->id = space->id_counter++;

    return 0;
}

int nvSpace_remove_rigidbody(phy_space *space, phy_rigid_body *body) {
    if (phy_array_remove(space->bodies, body) == (size_t)(-1)) return 1;

    // Remove broadphase pairs
    // This could break contacts if a remove call is made in an event callback
    for (size_t i = 0; i < space->broadphase_pairs->current_size; i++) {
        void *pool_i = (char *)space->broadphase_pairs->pool + i * space->broadphase_pairs->chunk_size;
        nvBroadPhasePair *pair = (nvBroadPhasePair *)pool_i;
        phy_rigid_body *body_a = pair->a;
        phy_rigid_body *body_b = pair->b;

        if (body_a == body || body_b == body) {
            pair->a = NULL;
            pair->b = NULL;
        }
    }

    // Remove contacts
    void *map_val;
    size_t map_iter = 0;
    while (phy_hashmap_iter(body->space->contacts, &map_iter, &map_val)) {
        phy_persistent_contact_pair *pcp = map_val;

        if (pcp->body_a == body || pcp->body_b == body) {
            nvPersistentContactPair_remove(body->space, pcp);
            map_iter = 0;
            continue;
        }
    }

    // Remove constraints
    phy_array *removed_constraints = phy_array_new();
    for (size_t i = 0; i < space->constraints->size; i++) {
        phy_constraint *cons = space->constraints->data[i];

        if (cons->a == body || cons->b == body)
            phy_array_add(removed_constraints, cons);
    }
    for (size_t i = 0; i < removed_constraints->size; i++) {
        phy_array_remove(space->constraints, removed_constraints->data[i]);
    }
    phy_array_free(removed_constraints);

    return 0;
}

int nvSpace_add_constraint(phy_space *space, phy_constraint *cons) {
    // TODO: This is inefficient
    for (size_t i = 0; i < space->constraints->size; i++) {
        phy_constraint *lcons = space->constraints->data[i];

        if (lcons == cons) {
            utl_error_func("Constraint already exists in space", utl_user_defined_data);
            return 2;
        }
    }

    return phy_array_add(space->constraints, cons);
}

int nvSpace_remove_constraint(phy_space *space, phy_constraint *cons) {
    if (phy_array_remove(space->constraints, cons) == (size_t)(-1))
        return 1;
    return 0;
}

nv_bool nvSpace_iter_bodies(phy_space *space, phy_rigid_body **body, size_t *index) {
    *body = space->bodies->data[(*index)++];
    return (*index <= space->bodies->size);
}

nv_bool nvSpace_iter_constraints(phy_space *space, phy_constraint **cons, size_t *index) {
    *cons = space->constraints->data[(*index)++];
    return (*index <= space->constraints->size);
}

void nvSpace_step(phy_space *space, nv_float dt) {
    if (dt == 0.0 || space->settings.substeps <= 0) return;
    nv_uint32 substeps = space->settings.substeps;
    nv_uint32 velocity_iters = space->settings.velocity_iterations;

    /*
        Simulation route
        ----------------
        1. Broadphase
        2. Narrowphase
        3. Integrate accelerations
        4. Solve constraints (PGS + Baumgarte)
        5. Integrate velocities
        6. Contact position correction (NGS)
    */

    NV_TRACY_ZONE_START;

    nvPrecisionTimer step_timer;
    NV_PROFILER_START(step_timer);

    nvPrecisionTimer timer;

    // For iterating contacts hashmap
    size_t l;
    void *map_val;

    dt /= (nv_float)substeps;
    nv_float inv_dt = 1.0 / dt;

    for (nv_uint32 substep = 0; substep < substeps; substep++) {
        /*
            Integrate accelerations
            -----------------------
            Apply forces, gravity, integrate accelerations (update velocities) and apply damping.
            We do this step first to reset body caches.
        */
        NV_PROFILER_START(timer);
        ITER_BODIES(body_i) {
            phy_rigid_body *body = (phy_rigid_body *)space->bodies->data[body_i];

            nvRigidBody_integrate_accelerations(body, space->gravity, dt);
        }
        NV_PROFILER_STOP(timer, space->profiler.integrate_accelerations);

        /*
            Broadphase
            ----------
            Generate possible collision pairs with the choosen broadphase algorithm.
        */
        NV_PROFILER_START(timer);
        switch (space->broadphase_algorithm) {
            case nvBroadPhaseAlg_BRUTE_FORCE:
                nv_broadphase_brute_force(space);
                break;

            case nvBroadPhaseAlg_BVH:
                nv_broadphase_BVH(space);
                break;
        }
        NV_PROFILER_STOP(timer, space->profiler.broadphase);

        NV_PROFILER_START(timer);
        nv_broadphase_finalize(space);
        NV_PROFILER_STOP(timer, space->profiler.broadphase_finalize);

        /*
            Narrowphase
            ------------
            Do narrow-phase checks between possible collision pairs and
            create & update contact pairs.
        */
        NV_PROFILER_START(timer);
        nv_narrow_phase(space);
        NV_PROFILER_STOP(timer, space->profiler.narrowphase);

        /*
            Solve constraints (PGS + Baumgarte)
            -----------------------------------
            Prepare velocity constraints, warm-start and solve iteratively.
            Use baumgarte depending on the position correction setting.

            Sequential Impulses / PGS + Baumgarte:
            https://box2d.org/files/ErinCatto_SequentialImpulses_GDC2006.pdf
        */

        // Prepare constraints for solving
        NV_PROFILER_START(timer);
        for (size_t i = 0; i < space->constraints->size; i++) {
            phy_constraint_presolve(
                space,
                (phy_constraint *)space->constraints->data[i],
                dt,
                inv_dt
            );
        }

        l = 0;
        while (phy_hashmap_iter(space->contacts, &l, &map_val)) {
            phy_persistent_contact_pair *pcp = map_val;
            phy_contact_presolve(space, pcp, inv_dt);
        }
        NV_PROFILER_STOP(timer, space->profiler.presolve);

        // Warmstart constraints
        NV_PROFILER_START(timer);
        for (size_t i = 0; i < space->constraints->size; i++) {
            phy_constraint_warmstart(
                space,
                (phy_constraint *)space->constraints->data[i]
            );
        }

        l = 0;
        while (phy_hashmap_iter(space->contacts, &l, &map_val)) {
            phy_persistent_contact_pair *pcp = map_val;
            phy_contact_warmstart(space, pcp);
        }
        NV_PROFILER_STOP(timer, space->profiler.warmstart);

        // Solve constraints iteratively
        NV_PROFILER_START(timer);
        for (size_t i = 0; i < velocity_iters; i++) {
            for (size_t j = 0; j < space->constraints->size; j++) {
                phy_constraint_solve(
                    (phy_constraint *)space->constraints->data[j],
                    inv_dt
                );
            }

            l = 0;
            while (phy_hashmap_iter(space->contacts, &l, &map_val)) {
                phy_persistent_contact_pair *pcp = map_val;
                phy_contact_solve_velocity(pcp);
            }
        }
        NV_PROFILER_STOP(timer, space->profiler.solve_velocities);

        /*
            Integrate velocities
            --------------------
            Integrate velocities (update positions) and check out-of-bound bodies.
        */
        NV_PROFILER_START(timer);
        ITER_BODIES(body_i) {
            phy_rigid_body *body = (phy_rigid_body *)space->bodies->data[body_i];

            nvRigidBody_integrate_velocities(body, dt);

            body->origin = nvVector2_sub(body->position, nvVector2_rotate(body->com, body->angle));

            // Reset caches
            if (body->type != PHY_RIGID_BODY_TYPE_STATIC) {
                body->cache_aabb = false;
                body->cache_transform = false;
            }
        }
        NV_PROFILER_STOP(timer, space->profiler.integrate_velocities);
    }
    
    NV_PROFILER_STOP(step_timer, space->profiler.step);

    NV_TRACY_ZONE_END;
    NV_TRACY_FRAMEMARK;
}

void nvSpace_cast_ray(
    phy_space *space,
    phy_vector2 from,
    phy_vector2 to,
    nvRayCastResult *results_array,
    size_t *num_hits,
    size_t capacity
) {
    /*
        TODO
        Ray checking order:
        BVH (or current bph) -> Shape AABBs -> Individual shapes
    */
    *num_hits = 0;

    phy_vector2 delta = nvVector2_sub(to, from);
    phy_vector2 dir = nvVector2_normalize(delta);
    nv_float maxsq = nvVector2_len2(delta);

    ITER_BODIES(body_i) {
        phy_rigid_body *body = space->bodies->data[body_i];
        nvTransform xform = {body->origin, body->angle};

        nvRayCastResult closest_result;
        nv_float min_dist = INFINITY;
        nv_float any_hit = false;

        for (size_t shape_i = 0; shape_i < body->shapes->size; shape_i++) {
            nvShape *shape = body->shapes->data[shape_i];

            nvRayCastResult result;
            nv_bool hit = false;

            switch (shape->type) {
                case nvShapeType_CIRCLE:
                    hit = nv_collide_ray_x_circle(&result, from, dir, maxsq, shape, xform);
                    break;

                case nvShapeType_POLYGON:
                    hit = nv_collide_ray_x_polygon(&result, from, dir, maxsq, shape, xform);
                    break;
            }

            if (hit) {
                any_hit = true;
                nv_float dist = nvVector2_dist2(from, result.position);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_result = result;
                }
            }
        }

        if (any_hit) {
            closest_result.body = body;
            results_array[(*num_hits)++] = closest_result;
            if ((*num_hits) == capacity) break;
        }
    }
}
