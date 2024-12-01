#pragma once

#include "khg_phy/aabb.h"
#include "khg_phy/material.h"
#include "khg_phy/shape.h"
#include "khg_phy/core/phy_array.h"
#include "khg_phy/core/phy_vector.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef enum {
  PHY_RIGID_BODY_TYPE_STATIC,
  PHY_RIGID_BODY_TYPE_DYNAMIC
} phy_rigid_body_type;

typedef struct phy_rigid_body {
  bool cache_aabb;
  bool cache_transform;
  phy_aabb cached_aabb;
  float bvh_median_x;
  float bvh_median_y;
  phy_vector2 force;
  float torque;
  float invmass;
  float invinertia;
  phy_vector2 origin;
  phy_vector2 com;
  void *user_data;
  struct phy_space *space;
  uint32_t id;
  phy_rigid_body_type type;
  phy_array *shapes;
  phy_vector2 position;
  float angle;
  phy_vector2 linear_velocity;
  float angular_velocity;
  float linear_damping_scale;
  float angular_damping_scale;
  float gravity_scale;
  phy_material material;
  float mass;
  float inertia;
  bool collision_enabled;
  uint32_t collision_group;
  uint32_t collision_category;
  uint32_t collision_mask;
} phy_rigid_body;

typedef struct phy_rigid_body_initializer {
  phy_rigid_body_type type;
  phy_vector2 position;
  float angle;
  phy_vector2 linear_velocity;
  float angular_velocity;
  phy_material material;
  void *user_data;
} phy_rigid_body_initializer;

static const phy_rigid_body_initializer phy_rigid_body_initializer_default = { PHY_RIGID_BODY_TYPE_STATIC, { 0.0, 0.0 }, 0.0, { 0.0, 0.0 }, 0.0, { 1.0, 0.1, 0.4 }, NULL };

phy_rigid_body *phy_rigid_body_new(phy_rigid_body_initializer init);
void phy_rigid_body_free(phy_rigid_body *body);

void phy_rigid_body_set_user_data(phy_rigid_body *body, void *data);
void *phy_rigid_body_get_user_data(const phy_rigid_body *body);

struct phy_space *phy_rigid_body_get_space(const phy_rigid_body *body);

uint32_t phy_rigid_body_get_id(const phy_rigid_body *body);

int phy_rigid_body_set_type(phy_rigid_body *body, phy_rigid_body_type type);
phy_rigid_body_type phy_rigid_body_get_type(const phy_rigid_body *body);

void phy_rigid_body_set_position(phy_rigid_body *body, phy_vector2 new_position);
phy_vector2 phy_rigid_body_get_position(const phy_rigid_body *body);

void phy_rigid_body_set_angle(phy_rigid_body *body, float new_angle);
float phy_rigid_body_get_angle(const phy_rigid_body *body);

void phy_rigid_body_set_linear_velocity(phy_rigid_body *body, phy_vector2 new_velocity);
phy_vector2 phy_rigid_body_get_linear_velocity(const phy_rigid_body *body);

void phy_rigid_body_set_angular_velocity(phy_rigid_body *body, float new_velocity);
float phy_rigid_body_get_angular_velocity(const phy_rigid_body *body);

void phy_rigid_body_set_linear_damping_scale(phy_rigid_body *body, float scale);
float phy_rigid_body_get_linear_damping_scale(const phy_rigid_body *body);

void phy_rigid_body_set_angular_damping_scale(phy_rigid_body *body, float scale);
float phy_rigid_body_get_angular_damping_scale(const phy_rigid_body *body);

void phy_rigid_body_set_gravity_scale(phy_rigid_body *body, float scale);
float phy_rigid_body_get_gravity_scale(const phy_rigid_body *body);

void phy_rigid_body_set_material(phy_rigid_body *body, phy_material material);
phy_material phy_rigid_body_get_material(const phy_rigid_body *body);

int phy_rigid_body_set_mass(phy_rigid_body *body, float mass);
float phy_rigid_body_get_mass(const phy_rigid_body *body);

void phy_rigid_body_set_inertia(phy_rigid_body *body, float inertia);
float phy_rigid_body_get_inertia(const phy_rigid_body *body);

void phy_rigid_body_set_collision_group(phy_rigid_body *body, uint32_t group);
uint32_t phy_rigid_body_get_collision_group(const phy_rigid_body *body);

void phy_rigid_body_set_collision_category(phy_rigid_body *body, uint32_t category);
uint32_t phy_rigid_body_get_collision_category(const phy_rigid_body *body);

void phy_rigid_body_set_collision_mask(phy_rigid_body *body, uint32_t mask);
uint32_t phy_rigid_body_get_collision_mask(const phy_rigid_body *body);

int phy_rigid_body_add_shape(phy_rigid_body *body, phy_shape *shape);
int phy_rigid_body_remove_shape(phy_rigid_body *body, phy_shape *shape);
bool phy_rigid_body_iter_shapes(phy_rigid_body *body, phy_shape **shape, size_t *index);

void phy_rigid_body_apply_force(phy_rigid_body *body, phy_vector2 force);
void phy_rigid_body_apply_force_at(phy_rigid_body *body, phy_vector2 force, phy_vector2 position);
void phy_rigid_body_apply_torque(phy_rigid_body *body, float torque);
void phy_rigid_body_apply_impulse(phy_rigid_body *body, phy_vector2 impulse, phy_vector2 position);

void phy_rigid_body_enable_collisions(phy_rigid_body *body);
void phy_rigid_body_disable_collisions(phy_rigid_body *body);

void phy_rigid_body_reset_velocities(phy_rigid_body *body);

phy_aabb phy_rigid_body_get_aabb(phy_rigid_body *body);

float phy_rigid_body_get_kinetic_energy(const phy_rigid_body *body);
float phy_rigid_body_get_rotational_energy(const phy_rigid_body *body);

void phy_rigid_body_integrate_accelerations(phy_rigid_body *body, phy_vector2 gravity, float dt);
void phy_rigid_body_integrate_velocities(phy_rigid_body *body, float dt);

