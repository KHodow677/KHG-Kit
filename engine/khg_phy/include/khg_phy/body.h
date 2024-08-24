#pragma once

#include "khg_phy/phy_types.h"

typedef enum phy_body_type {
	PHY_BODY_TYPE_DYNAMIC,
	PHY_BODY_TYPE_KINEMATIC,
	PHY_BODY_TYPE_STATIC,
} phy_body_type;

typedef void (*phy_body_velocity_func)(phy_body *body, phy_vect gravity, float damping, float dt);
typedef void (*phy_body_position_func)(phy_body *body, float dt);

phy_body *phy_body_alloc(void);
phy_body *phy_body_init(phy_body *body, float mass, float moment);
phy_body *phy_body_new(float mass, float moment);

phy_body *phy_body_new_kinematic(void);
phy_body *phy_body_new_static(void);

void phy_body_destroy(phy_body *body);
void phy_body_free(phy_body *body);

void phy_body_activate(phy_body *body);
void phy_body_activate_static(phy_body *body, phy_shape *filter);

void phy_body_sleep(phy_body *body);
void phy_body_sleep_with_group(phy_body *body, phy_body *group);

bool phy_body_is_sleeping(const phy_body *body);

phy_body_type phy_body_get_type(phy_body *body);
void phy_body_set_type(phy_body *body, phy_body_type type);

phy_space *phy_body_get_space(const phy_body *body);

float phy_body_get_mass(const phy_body *body);
void phy_body_set_mass(phy_body *body, float m);

float phy_body_get_moment(const phy_body *body);
void phy_body_set_moment(phy_body *body, float i);

phy_vect phy_body_get_position(const phy_body *body);
void phy_body_set_position(phy_body *body, phy_vect pos);

phy_vect phy_body_get_center_of_gravity(const phy_body *body);
void phy_body_set_center_of_gravity(phy_body *body, phy_vect cog);

phy_vect phy_body_get_velocity(const phy_body *body);
void phy_body_set_velocity(phy_body *body, phy_vect velocity);

phy_vect phy_body_get_force(const phy_body *body);
void phy_body_set_force(phy_body *body, phy_vect force);

float phy_body_get_angle(const phy_body *body);
void phy_body_set_angle(phy_body *body, float a);

float phy_body_get_angular_velocity(const phy_body *body);
void phy_body_set_angular_velocity(phy_body *body, float angular_velocity);

float phy_body_get_torque(const phy_body *body);
void phy_body_set_torque(phy_body *body, float torque);

phy_vect phy_body_get_rotation(const phy_body *body);

phy_data_pointer phy_body_get_user_data(const phy_body *body);
void phy_body_set_user_data(phy_body *body, phy_data_pointer user_data);

void phy_body_set_velocity_update_func(phy_body *body, phy_body_velocity_func velocity_func);
void phy_body_set_position_update_func(phy_body *body, phy_body_position_func position_func);

void phy_body_update_velocity(phy_body *body, phy_vect gravity, float damping, float dt);
void phy_body_update_position(phy_body *body, float dt);

phy_vect phy_body_local_to_world(const phy_body *body, const phy_vect point);
phy_vect phy_body_world_to_local(const phy_body *body, const phy_vect point);

void phy_body_apply_force_at_world_point(phy_body *body, phy_vect force, phy_vect point);
void phy_body_apply_force_at_local_point(phy_body *body, phy_vect force, phy_vect point);

void phy_body_apply_impulse_at_world_point(phy_body *body, phy_vect impulse, phy_vect point);
void phy_body_apply_impulse_at_local_point(phy_body *body, phy_vect impulse, phy_vect point);

phy_vect phy_body_get_velocity_at_world_point(const phy_body *body, phy_vect point);
phy_vect phy_body_get_velocity_at_local_point(const phy_body *body, phy_vect point);

float phy_body_kinetic_energy(const phy_body *body);

typedef void (*phy_body_shape_iterator_func)(phy_body *body, phy_shape *shape, void *data);
void phy_body_each_shape(phy_body *body, phy_body_shape_iterator_func func, void *data);

typedef void (*phy_body_constraint_iterator_func)(phy_body *body, phy_constraint *constraint, void *data);
void phy_body_each_constraint(phy_body *body, phy_body_constraint_iterator_func func, void *data);

typedef void (*phy_body_arbiter_iterator_func)(phy_body *body, phy_arbiter *arbiter, void *data);
void phy_body_each_arbiter(phy_body *body, phy_body_arbiter_iterator_func func, void *data);

