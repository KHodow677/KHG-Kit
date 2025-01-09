#include "khg_phy/body.h"
#include "khg_phy/aabb.h"
#include "khg_phy/math.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_array.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>

static int phy_rigid_body_accumulate_mass(phy_rigid_body *body) {
  body->mass = 0.0;
  body->invmass = 0.0;
  body->inertia = 0.0;
  body->invinertia = 0.0;
  if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
    return 0;
  }
  phy_vector2 local_com = phy_vector2_zero;
  for (unsigned int i = 0; i < body->shapes->size; i++) {
    phy_shape *shape = body->shapes->data[i];
    phy_shape_mass_info mass_info = phy_shape_calculate_mass(shape, body->material.density);
    body->mass += mass_info.mass;
    body->inertia += mass_info.inertia;
    local_com = phy_vector2_add(local_com, phy_vector2_mul(mass_info.center, mass_info.mass));
  }
  if (body->mass == 0.0) {
    return 1;
  }
  body->invmass = 1.0 / body->mass;
  local_com = phy_vector2_mul(local_com, body->invmass);
  body->inertia -= body->mass * phy_vector2_dot(local_com, local_com);
  if (body->inertia == 0.0) {
    utl_error_func("Invalid mass", utl_user_defined_data);
    return 1;
  }
  body->invinertia = 1.0 / body->inertia;
  body->com = local_com;
  body->position = phy_vector2_add(phy_vector2_rotate(body->com, body->angle), body->origin);
  return 0;
}

phy_rigid_body *phy_rigid_body_new(phy_rigid_body_initializer init) {
  phy_rigid_body *body = malloc(sizeof(phy_rigid_body));
  if (!body) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  body->user_data = init.user_data;
  body->space = NULL;
  body->type = init.type;
  body->shapes = phy_array_new();
  if (!body->shapes) {
    free(body);
    return NULL;
  }
  body->origin = init.position;
  body->position = init.position;
  body->angle = init.angle;
  body->linear_velocity = init.linear_velocity;
  body->angular_velocity = init.angular_velocity;
  body->linear_damping_scale = 1.0;
  body->angular_damping_scale = 1.0;
  body->force = phy_vector2_zero;
  body->torque = 0.0;
  body->gravity_scale = 1.0;
  body->com = phy_vector2_zero;
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

void phy_rigid_body_free(phy_rigid_body *body) {
  if (!body) {
    return;
  }
  for (unsigned int i = 0; i < body->shapes->size; i++) {
    phy_shape_free(body->shapes->data[i]);
  }
  phy_array_free(body->shapes);
  free(body);
}

void phy_rigid_body_set_user_data(phy_rigid_body *body, void *data) {
  body->user_data = data;
}

void *phy_rigid_body_get_user_data(const phy_rigid_body *body) {
  return body->user_data;
}

phy_space *phy_rigid_body_get_space(const phy_rigid_body *body) {
  return body->space;
}

unsigned int phy_rigid_body_get_id(const phy_rigid_body *body) {
  return body->id;
}

int phy_rigid_body_set_type(phy_rigid_body *body, phy_rigid_body_type type) {
  phy_rigid_body_type old_type = body->type;
  body->type = type;
  if (old_type == PHY_RIGID_BODY_TYPE_STATIC && type == PHY_RIGID_BODY_TYPE_DYNAMIC) {
    return phy_rigid_body_accumulate_mass(body);
  }
  return 0;
}

phy_rigid_body_type phy_rigid_body_get_type(const phy_rigid_body *body) {
  return body->type;
}

void phy_rigid_body_set_position(phy_rigid_body *body, phy_vector2 new_position) {
  body->position = new_position;
  body->origin = phy_vector2_add(phy_vector2_rotate(body->com, body->angle), body->position);
  body->cache_aabb = false;
  body->cache_transform = false;
}

phy_vector2 phy_rigid_body_get_position(const phy_rigid_body *body) {
  return body->position;
}

void phy_rigid_body_set_angle(phy_rigid_body *body, float new_angle) {
  body->angle = new_angle;
  body->origin = phy_vector2_add(phy_vector2_rotate(body->com, body->angle), body->position);
  body->cache_aabb = false;
  body->cache_transform = false;
}

float phy_rigid_body_get_angle(const phy_rigid_body *body) {
  return body->angle;
}

void phy_rigid_body_set_linear_velocity(phy_rigid_body *body, phy_vector2 new_velocity) {
  body->linear_velocity = new_velocity;
}

phy_vector2 phy_rigid_body_get_linear_velocity(const phy_rigid_body *body) {
  return body->linear_velocity;
}

void phy_rigid_body_set_angular_velocity(phy_rigid_body *body, float new_velocity) {
  body->angular_velocity = new_velocity;
}

float phy_rigid_body_get_angular_velocity(const phy_rigid_body *body) {
  return body->angular_velocity;
}

void phy_rigid_body_set_linear_damping_scale(phy_rigid_body *body, float scale) {
  body->linear_damping_scale = scale;
}

float phy_rigid_body_get_linear_damping_scale(const phy_rigid_body *body) {
  return body->linear_damping_scale;
}

void phy_rigid_body_set_angular_damping_scale(phy_rigid_body *body, float scale) {
  body->angular_damping_scale = scale;
}

float phy_rigid_body_get_angular_damping_scale(const phy_rigid_body *body) {
  return body->angular_damping_scale;
}

void phy_rigid_body_set_gravity_scale(phy_rigid_body *body, float scale) {
  body->gravity_scale = scale;
}

float phy_rigid_body_get_gravity_scale(const phy_rigid_body *body) {
  return body->gravity_scale;
}

void phy_rigid_body_set_material(phy_rigid_body *body, phy_material material) {
  body->material = material;
  phy_rigid_body_accumulate_mass(body);
}

phy_material phy_rigid_body_get_material(const phy_rigid_body *body) {
  return body->material;
}

int phy_rigid_body_set_mass(phy_rigid_body *body, float mass) {
  if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
    return 0;
  }
  if (mass == 0.0) {
    return 1;
  }
  body->mass = mass;
  body->invmass = 1.0 / body->mass;
  return 0;
}

float phy_rigid_body_get_mass(const phy_rigid_body *body) {
  return body->mass;
}

void phy_rigid_body_set_inertia(phy_rigid_body *body, float inertia) {
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

float phy_rigid_body_get_inertia(const phy_rigid_body *body) {
  return body->inertia;
}

void phy_rigid_body_set_collision_group(phy_rigid_body *body, unsigned int group) {
  body->collision_group = group;
}

unsigned int phy_rigid_body_get_collision_group(const phy_rigid_body *body) {
  return body->collision_group;
}

void phy_rigid_body_set_collision_category(phy_rigid_body *body, unsigned int category) {
  body->collision_category = category;
}

unsigned int phy_rigid_body_get_collision_category(const phy_rigid_body *body) {
  return body->collision_category;
}

void phy_rigid_body_set_collision_mask(phy_rigid_body *body, unsigned int mask) {
  body->collision_mask = mask;
}

unsigned int phy_rigid_body_get_collision_mask(const phy_rigid_body *body) {
  return body->collision_mask;
}

int phy_rigid_body_add_shape(phy_rigid_body *body, phy_shape *shape) {
  if (phy_array_add(body->shapes, shape)) {
    return 1;
  }
  if (phy_rigid_body_accumulate_mass(body)) {
    return 2;
  }
  return 0;
}

int phy_rigid_body_remove_shape(phy_rigid_body *body, phy_shape *shape) {
  if (phy_array_remove(body->shapes, shape) == (unsigned int)(-1)) {
    return 1;
  }
  if (phy_rigid_body_accumulate_mass(body)) {
    return 2;
  }
  void *map_val;
  unsigned int map_iter = 0;
  while (phy_hashmap_iter(body->space->contacts, &map_iter, &map_val)) {
    phy_persistent_contact_pair *pcp = map_val;
    for (unsigned int i = 0; i < body->shapes->size; i++) {
      phy_shape *shape = body->shapes->data[i];
      if ((pcp->body_a == body && shape == pcp->shape_a) || (pcp->body_b == body && shape == pcp->shape_b)) {
        phy_persistent_contact_pair_remove(body->space, pcp);
        break;
      }
    }
  }
  return 0;
}

bool phy_rigid_body_iter_shapes(phy_rigid_body *body, phy_shape **shape, unsigned int *index) {
  *shape = body->shapes->data[(*index)++];
  return (*index <= body->shapes->size);
}

void phy_rigid_body_apply_force(phy_rigid_body *body, phy_vector2 force) {
  if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
    return;
  }
  body->force = phy_vector2_add(body->force, force);
}

void phy_rigid_body_apply_force_at(phy_rigid_body *body, phy_vector2 force, phy_vector2 position) {
  if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
    return;
  }
  body->force = phy_vector2_add(body->force, force);
  body->torque += phy_vector2_cross(position, force);
}

void phy_rigid_body_apply_torque(phy_rigid_body *body, float torque) {
  if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
    return;
  }
  body->torque += torque;
}

void phy_rigid_body_apply_impulse(phy_rigid_body *body, phy_vector2 impulse, phy_vector2 position) {
  if ((body)->type != PHY_RIGID_BODY_TYPE_DYNAMIC) {
    return;
  }
  body->linear_velocity = phy_vector2_add(body->linear_velocity, phy_vector2_mul(impulse, body->invmass));
  body->angular_velocity += phy_vector2_cross(position, impulse) * body->invinertia;
}

void phy_rigid_body_enable_collisions(phy_rigid_body *body) {
  body->collision_enabled = true;
}

void phy_rigid_body_disable_collisions(phy_rigid_body *body) {
  body->collision_enabled = false;
}

void phy_rigid_body_reset_velocities(phy_rigid_body *body) {
  phy_rigid_body_set_linear_velocity(body, phy_vector2_zero);
  phy_rigid_body_set_angular_velocity(body, 0.0);
  body->force = phy_vector2_zero;
  body->torque = 0.0;
}

phy_aabb phy_rigid_body_get_aabb(phy_rigid_body *body) {
  if (body->cache_aabb) {
    return body->cached_aabb;
  }
  body->cache_aabb = true;
  phy_transform xform = (phy_transform){body->origin, body->angle};
  phy_aabb total_aabb = phy_shape_get_aabb(body->shapes->data[0], xform);
  for (unsigned int i = 1; i < body->shapes->size; i++) {
    total_aabb = nvAABB_merge(total_aabb, phy_shape_get_aabb(body->shapes->data[i], xform));
  }
  body->cached_aabb = total_aabb;
  return total_aabb;
}

float phy_rigid_body_get_kinetic_energy(const phy_rigid_body *body) {
  return 0.5 * body->mass * phy_vector2_len2(body->linear_velocity);
}

float phy_rigid_body_get_rotational_energy(const phy_rigid_body *body) {
  return 0.5 * body->inertia * fabsf(body->angular_velocity);
}

void phy_rigid_body_integrate_accelerations(phy_rigid_body *body, phy_vector2 gravity, float dt) {
  if (body->type == PHY_RIGID_BODY_TYPE_STATIC) {
    phy_rigid_body_reset_velocities(body);
    return;
  }
  phy_vector2 linear_acceleration = phy_vector2_add(phy_vector2_mul(body->force, body->invmass), phy_vector2_mul(gravity, body->gravity_scale));
  body->linear_velocity = phy_vector2_add(body->linear_velocity, phy_vector2_mul(linear_acceleration, dt));
  float angular_acceleration = body->torque * body->invinertia;
  body->angular_velocity += angular_acceleration * dt;
  float kv = powf(0.99, body->linear_damping_scale * body->space->settings.linear_damping);
  float ka = powf(0.99, body->angular_damping_scale * body->space->settings.angular_damping);
  body->linear_velocity = phy_vector2_mul(body->linear_velocity, kv);
  body->angular_velocity *= ka;
}

void phy_rigid_body_integrate_velocities(phy_rigid_body *body, float dt) {
  if (body->type == PHY_RIGID_BODY_TYPE_STATIC) {
    phy_rigid_body_reset_velocities(body);
    return;
  }
  body->position = phy_vector2_add(body->position, phy_vector2_mul(body->linear_velocity, dt));
  body->angle += body->angular_velocity * dt;
  body->force = phy_vector2_zero;
  body->torque = 0.0;
}

