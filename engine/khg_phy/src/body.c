/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include <stdlib.h>
#include "khg_phy/body.h"
#include "khg_phy/core/phy_array.h"
#include "khg_phy/math.h"
#include "khg_phy/aabb.h"
#include "khg_phy/constants.h"
#include "khg_phy/space.h"
#include "khg_utl/error_func.h"


/**
 * @file body.c
 * 
 * @brief Rigid body implementation.
 */



phy_rigid_body *nvRigidBody_new(phy_rigid_body_initializer init) {
  phy_rigid_body *body = NV_NEW(phy_rigid_body);
  if (!body) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }

    body->user_data = init.user_data;

    body->space = NULL;

    body->type = init.type;

    body->shapes = phy_array_new();
    if (!body->shapes) {
        NV_FREE(body);
        return NULL;
    }

    body->origin = init.position;
    body->position = init.position;
    body->angle = init.angle;

    body->linear_velocity = init.linear_velocity;
    body->angular_velocity = init.angular_velocity;

    body->linear_damping_scale = 1.0;
    body->angular_damping_scale = 1.0;

    body->force = nvVector2_zero;
    body->torque = 0.0;

    body->gravity_scale = 1.0;
    body->com = nvVector2_zero;

    body->material = init.material;

    body->collision_enabled = true;
    body->collision_group = 0;
    body->collision_category = 0b11111111111111111111111111111111;
    body->collision_mask = 0b11111111111111111111111111111111;

    body->cache_aabb = false;
    body->cache_transform = false;
    body->cached_aabb = (phy_aabb){0.0, 0.0, 0.0, 0.0};

    return body;
}

void nvRigidBody_free(phy_rigid_body *body) {
    if (!body) return;

    for (size_t i = 0; i < body->shapes->size; i++) {
        nvShape_free(body->shapes->data[i]);
    }
    phy_array_free(body->shapes);
    
    NV_FREE(body);
}

static int nvRigidBody_accumulate_mass(phy_rigid_body *body) {
    body->mass = 0.0;
    body->invmass = 0.0;
    body->inertia = 0.0;
    body->invinertia = 0.0;

    if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
      return 0;
    }

    // Accumulate mass information from shapes

    phy_vector2 local_com = nvVector2_zero;
    for (size_t i = 0; i < body->shapes->size; i++) {
        nvShape *shape = body->shapes->data[i];

        nvShapeMassInfo mass_info = nvShape_calculate_mass(shape, body->material.density);

        body->mass += mass_info.mass;
        body->inertia += mass_info.inertia;
        local_com = nvVector2_add(local_com, nvVector2_mul(mass_info.center, mass_info.mass));
    }

    if (body->mass == 0.0) {
      utl_error_func("Dynamic bodies with no mass", utl_user_defined_data);
      return 1;
    }

    // Calculate center of mass and center the inertia

    body->invmass = 1.0 / body->mass;
    local_com = nvVector2_mul(local_com, body->invmass);

    body->inertia -= body->mass * nvVector2_dot(local_com, local_com);
    if (body->inertia == 0.0) {
      utl_error_func("Invalid mass", utl_user_defined_data);
      return 1;
    }
    body->invinertia = 1.0 / body->inertia;

    body->com = local_com;
    body->position = nvVector2_add(nvVector2_rotate(body->com, body->angle), body->origin);

    return 0;
}

void nvRigidBody_set_user_data(phy_rigid_body *body, void *data) {
    body->user_data = data;
}

void *nvRigidBody_get_user_data(const phy_rigid_body *body) {
    return body->user_data;
}

phy_space *nvRigidBody_get_space(const phy_rigid_body *body) {
    return body->space;
}

nv_uint32 nvRigidBody_get_id(const phy_rigid_body *body) {
    return body->id;
}

int nvRigidBody_set_type(phy_rigid_body *body, phy_rigid_body_type type) {
    phy_rigid_body_type old_type = body->type;
    body->type = type;

    // If the body was static from start the mass info might have not been calculated
    if (old_type == PHY_RIGID_BODY_TYPE_STATIC && type == PHY_RIGID_BODY_TYPE_DYNAMIC)
        return nvRigidBody_accumulate_mass(body);

    return 0;
}

phy_rigid_body_type nvRigidBody_get_type(const phy_rigid_body *body) {
    return body->type;
}

void nvRigidBody_set_position(phy_rigid_body *body, phy_vector2 new_position) {
    body->position = new_position;
    body->origin = nvVector2_add(nvVector2_rotate(body->com, body->angle), body->position);
    body->cache_aabb = false;
    body->cache_transform = false;
}

phy_vector2 nvRigidBody_get_position(const phy_rigid_body *body) {
    return body->position;
}

void nvRigidBody_set_angle(phy_rigid_body *body, nv_float new_angle) {
    body->angle = new_angle;
    body->origin = nvVector2_add(nvVector2_rotate(body->com, body->angle), body->position);
    body->cache_aabb = false;
    body->cache_transform = false;
}

nv_float nvRigidBody_get_angle(const phy_rigid_body *body) {
    return body->angle;
}

void nvRigidBody_set_linear_velocity(phy_rigid_body *body, phy_vector2 new_velocity) {
    body->linear_velocity = new_velocity;
}

phy_vector2 nvRigidBody_get_linear_velocity(const phy_rigid_body *body) {
    return body->linear_velocity;
}

void nvRigidBody_set_angular_velocity(phy_rigid_body *body, nv_float new_velocity) {
    body->angular_velocity = new_velocity;
}

nv_float nvRigidBody_get_angular_velocity(const phy_rigid_body *body) {
    return body->angular_velocity;
}

void nvRigidBody_set_linear_damping_scale(phy_rigid_body *body, nv_float scale) {
    body->linear_damping_scale = scale;
}

nv_float nvRigidBody_get_linear_damping_scale(const phy_rigid_body *body) {
    return body->linear_damping_scale;
}

void nvRigidBody_set_angular_damping_scale(phy_rigid_body *body, nv_float scale) {
    body->angular_damping_scale = scale;
}

nv_float nvRigidBody_get_angular_damping_scale(const phy_rigid_body *body) {
    return body->angular_damping_scale;
}

void nvRigidBody_set_gravity_scale(phy_rigid_body *body, nv_float scale) {
    body->gravity_scale = scale;
}

nv_float nvRigidBody_get_gravity_scale(const phy_rigid_body *body) {
    return body->gravity_scale;
}

void nvRigidBody_set_material(phy_rigid_body *body, nvMaterial material) {
    body->material = material;
    nvRigidBody_accumulate_mass(body);
}

nvMaterial nvRigidBody_get_material(const phy_rigid_body *body) {
    return body->material;
}

int nvRigidBody_set_mass(phy_rigid_body *body, nv_float mass) {
    if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
      return 0;
    }

    if (mass == 0.0) {
      utl_error_func("Dynamic bodies with no mass", utl_user_defined_data);
      return 1;
    }

    body->mass = mass;
    body->invmass = 1.0 / body->mass;

    // TODO: Recalculate inertia from shapes with updated mass?

    return 0;
}

nv_float nvRigidBody_get_mass(const phy_rigid_body *body) {
    return body->mass;
}

void nvRigidBody_set_inertia(phy_rigid_body *body, nv_float inertia) {
    if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
      return;
    }

    if (inertia == 0.0) {
        body->inertia = 0.0;
        body->invinertia = 0.0;
    }
    else {
        body->inertia = inertia;
        body->invinertia = 1.0 / inertia;
    }
}

nv_float nvRigidBody_get_inertia(const phy_rigid_body *body) {
    return body->inertia;
}

void nvRigidBody_set_collision_group(phy_rigid_body *body, nv_uint32 group) {
    body->collision_group = group;
}

nv_uint32 nvRigidBody_get_collision_group(const phy_rigid_body *body) {
    return body->collision_group;
}

void nvRigidBody_set_collision_category(phy_rigid_body *body, nv_uint32 category) {
    body->collision_category = category;
}

nv_uint32 nvRigidBody_get_collision_category(const phy_rigid_body *body) {
    return body->collision_category;
}

void nvRigidBody_set_collision_mask(phy_rigid_body *body, nv_uint32 mask) {
    body->collision_mask = mask;
}

nv_uint32 nvRigidBody_get_collision_mask(const phy_rigid_body *body) {
    return body->collision_mask;
}

int nvRigidBody_add_shape(phy_rigid_body *body, nvShape *shape) {
    if (phy_array_add(body->shapes, shape)) return 1;

    if (nvRigidBody_accumulate_mass(body)) return 2;

    return 0;
}

int nvRigidBody_remove_shape(phy_rigid_body *body, nvShape *shape) {
    if (phy_array_remove(body->shapes, shape) == (size_t)(-1)) return 1;

    if (nvRigidBody_accumulate_mass(body)) return 2;

    // Remove contacts
    void *map_val;
    size_t map_iter = 0;
    while (phy_hashmap_iter(body->space->contacts, &map_iter, &map_val)) {
        phy_persistent_contact_pair *pcp = map_val;

        for (size_t i = 0; i < body->shapes->size; i++) {
            nvShape *shape = body->shapes->data[i];

            if (
                (pcp->body_a == body && shape == pcp->shape_a) ||
                (pcp->body_b == body && shape == pcp->shape_b)
            ) {
                nvPersistentContactPair_remove(body->space, pcp);
                break;
            }
        }
    }

    return 0;
}

bool nvRigidBody_iter_shapes(phy_rigid_body *body, nvShape **shape, size_t *index) {
    *shape = body->shapes->data[(*index)++];
    return (*index <= body->shapes->size);
}

void nvRigidBody_apply_force(phy_rigid_body *body, phy_vector2 force) {
    if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
      return;
    }

    body->force = nvVector2_add(body->force, force);
}

void nvRigidBody_apply_force_at(
    phy_rigid_body *body,
    phy_vector2 force,
    phy_vector2 position
) {
    if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
      return;
    }

    body->force = nvVector2_add(body->force, force);
    body->torque += nvVector2_cross(position, force);
}

void nvRigidBody_apply_torque(phy_rigid_body *body, nv_float torque) {
    if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
      return;
    }

    body->torque += torque;
}

void nvRigidBody_apply_impulse(
    phy_rigid_body *body,
    phy_vector2 impulse,
    phy_vector2 position
) {
    if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
      return;
    }

    /*
        v -= J * (1/M)
        w -= rᴾ ⨯ J * (1/I)
    */

    body->linear_velocity = nvVector2_add(
        body->linear_velocity, nvVector2_mul(impulse, body->invmass));

    body->angular_velocity += nvVector2_cross(position, impulse) * body->invinertia;
}

void nvRigidBody_enable_collisions(phy_rigid_body *body) {
    body->collision_enabled = true;
}

void nvRigidBody_disable_collisions(phy_rigid_body *body) {
    body->collision_enabled = false;
}

void nvRigidBody_reset_velocities(phy_rigid_body *body) {
    nvRigidBody_set_linear_velocity(body, nvVector2_zero);
    nvRigidBody_set_angular_velocity(body, 0.0);
    body->force = nvVector2_zero;
    body->torque = 0.0;
}

phy_aabb nvRigidBody_get_aabb(phy_rigid_body *body) {
    NV_TRACY_ZONE_START;

    if (body->cache_aabb) {
        NV_TRACY_ZONE_END;
        return body->cached_aabb;
    }

    body->cache_aabb = true;

    nvTransform xform = (nvTransform){body->origin, body->angle};
    phy_aabb total_aabb = nvShape_get_aabb(body->shapes->data[0], xform);
    for (size_t i = 1; i < body->shapes->size; i++) {
        total_aabb = nvAABB_merge(total_aabb, nvShape_get_aabb(body->shapes->data[i], xform));
    }

    body->cached_aabb = total_aabb;

    NV_TRACY_ZONE_END;
    return total_aabb;
}

nv_float nvRigidBody_get_kinetic_energy(const phy_rigid_body *body) {
    // 1/2 * M * v²
    return 0.5 * body->mass * nvVector2_len2(body->linear_velocity);
}

nv_float nvRigidBody_get_rotational_energy(const phy_rigid_body *body) {
    // 1/2 * I * ω²
    return 0.5 * body->inertia * nv_fabs(body->angular_velocity);
}

void nvRigidBody_integrate_accelerations(
    phy_rigid_body *body,
    phy_vector2 gravity,
    nv_float dt
) {
    if (body->type == PHY_RIGID_BODY_TYPE_STATIC) {
        nvRigidBody_reset_velocities(body);
        return;
    }
    NV_TRACY_ZONE_START;

    // Semi-Implicit Euler Integration
    
    /*
        Integrate linear acceleration

        a = F * (1/M) + g
        v = a * Δt
    */
    phy_vector2 linear_acceleration = nvVector2_add(
        nvVector2_mul(body->force, body->invmass), nvVector2_mul(gravity, body->gravity_scale));

    body->linear_velocity = nvVector2_add(
        body->linear_velocity, nvVector2_mul(linear_acceleration, dt));

    /*
        Integrate angular acceleration
        
        α = T * (1/I)
        ω = α * Δt
    */
    nv_float angular_acceleration = body->torque * body->invinertia;
    body->angular_velocity += angular_acceleration * dt;

    // Dampen velocities
    nv_float kv = nv_pow(0.99, body->linear_damping_scale * body->space->settings.linear_damping);
    nv_float ka = nv_pow(0.99, body->angular_damping_scale * body->space->settings.angular_damping);
    body->linear_velocity = nvVector2_mul(body->linear_velocity, kv);
    body->angular_velocity *= ka;

    NV_TRACY_ZONE_END;
}

void nvRigidBody_integrate_velocities(phy_rigid_body *body, nv_float dt) {
    if (body->type == PHY_RIGID_BODY_TYPE_STATIC) {
        nvRigidBody_reset_velocities(body);
        return;
    }
    NV_TRACY_ZONE_START;

    // Semi-Implicit Euler Integration

    /*
        Integrate linear velocity

        x = v * Δt
    */
    body->position = nvVector2_add(body->position, nvVector2_mul(body->linear_velocity, dt));

    /*
        Integrate angular velocity

        θ = ω * Δt
    */
    body->angle += body->angular_velocity * dt;

    body->force = nvVector2_zero;
    body->torque = 0.0;

    NV_TRACY_ZONE_END;
}
