#pragma once

#include "khg_phy/core/phy_array.h"
#include "khg_phy/vector.h"
#include "khg_phy/aabb.h"
#include "khg_phy/material.h"
#include "khg_phy/shape.h"
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
  nvMaterial material;
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
  nvMaterial material;
  void *user_data;
} phy_rigid_body_initializer;

static const phy_rigid_body_initializer nvRigidBodyInitializer_default = { PHY_RIGID_BODY_TYPE_STATIC, { 0.0, 0.0 }, 0.0, { 0.0, 0.0 }, 0.0, { 1.0, 0.1, 0.4 }, NULL };

phy_rigid_body *nvRigidBody_new(phy_rigid_body_initializer init);
void nvRigidBody_free(phy_rigid_body *body);

void nvRigidBody_set_user_data(phy_rigid_body *body, void *data);
void *nvRigidBody_get_user_data(const phy_rigid_body *body);

struct phy_space *nvRigidBody_get_space(const phy_rigid_body *body);

uint32_t nvRigidBody_get_id(const phy_rigid_body *body);

int nvRigidBody_set_type(phy_rigid_body *body, phy_rigid_body_type type);
phy_rigid_body_type nvRigidBody_get_type(const phy_rigid_body *body);

void nvRigidBody_set_position(phy_rigid_body *body, phy_vector2 new_position);
phy_vector2 nvRigidBody_get_position(const phy_rigid_body *body);

void nvRigidBody_set_angle(phy_rigid_body *body, float new_angle);
float nvRigidBody_get_angle(const phy_rigid_body *body);

void nvRigidBody_set_linear_velocity(phy_rigid_body *body, phy_vector2 new_velocity);
phy_vector2 nvRigidBody_get_linear_velocity(const phy_rigid_body *body);

void nvRigidBody_set_angular_velocity(phy_rigid_body *body, float new_velocity);
float nvRigidBody_get_angular_velocity(const phy_rigid_body *body);

void nvRigidBody_set_linear_damping_scale(phy_rigid_body *body, float scale);
float nvRigidBody_get_linear_damping_scale(const phy_rigid_body *body);

void nvRigidBody_set_angular_damping_scale(phy_rigid_body *body, float scale);
nv_float nvRigidBody_get_angular_damping_scale(const phy_rigid_body *body);

void nvRigidBody_set_gravity_scale(phy_rigid_body *body, float scale);
float nvRigidBody_get_gravity_scale(const phy_rigid_body *body);

void nvRigidBody_set_material(phy_rigid_body *body, nvMaterial material);
nvMaterial nvRigidBody_get_material(const phy_rigid_body *body);

int nvRigidBody_set_mass(phy_rigid_body *body, float mass);
float nvRigidBody_get_mass(const phy_rigid_body *body);

void nvRigidBody_set_inertia(phy_rigid_body *body, float inertia);
float nvRigidBody_get_inertia(const phy_rigid_body *body);

void nvRigidBody_set_collision_group(phy_rigid_body *body, uint32_t group);
uint32_t nvRigidBody_get_collision_group(const phy_rigid_body *body);

void nvRigidBody_set_collision_category(phy_rigid_body *body, uint32_t category);
uint32_t nvRigidBody_get_collision_category(const phy_rigid_body *body);

void nvRigidBody_set_collision_mask(phy_rigid_body *body, uint32_t mask);
uint32_t nvRigidBody_get_collision_mask(const phy_rigid_body *body);

int nvRigidBody_add_shape(phy_rigid_body *body, nvShape *shape);
int nvRigidBody_remove_shape(phy_rigid_body *body, nvShape *shape);
bool nvRigidBody_iter_shapes(phy_rigid_body *body, nvShape **shape, size_t *index);

void nvRigidBody_apply_force(phy_rigid_body *body, phy_vector2 force);
void nvRigidBody_apply_force_at(phy_rigid_body *body, phy_vector2 force, phy_vector2 position);
void nvRigidBody_apply_torque(phy_rigid_body *body, float torque);
void nvRigidBody_apply_impulse(phy_rigid_body *body, phy_vector2 impulse, phy_vector2 position);

void nvRigidBody_enable_collisions(phy_rigid_body *body);
void nvRigidBody_disable_collisions(phy_rigid_body *body);

void nvRigidBody_reset_velocities(phy_rigid_body *body);

phy_aabb nvRigidBody_get_aabb(phy_rigid_body *body);

float nvRigidBody_get_kinetic_energy(const phy_rigid_body *body);
float nvRigidBody_get_rotational_energy(const phy_rigid_body *body);

void nvRigidBody_integrate_accelerations(phy_rigid_body *body, phy_vector2 gravity, float dt);
void nvRigidBody_integrate_velocities(phy_rigid_body *body, float dt);

