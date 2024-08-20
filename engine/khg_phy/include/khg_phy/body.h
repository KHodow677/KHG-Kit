#pragma once

#include "khg_phy/phy_types.h"

typedef enum phy_body_type {
	CP_BODY_TYPE_DYNAMIC,
	CP_BODY_TYPE_KINEMATIC,
	CP_BODY_TYPE_STATIC,
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
void phy_body_set_angular_velocity(phy_body *body, float angularVelocity);

/// Get the torque applied to the body for the next time step.
float cpBodyGetTorque(const phy_body *body);
/// Set the torque applied to the body for the next time step.
void cpBodySetTorque(phy_body *body, float torque);

/// Get the rotation vector of the body. (The x basis vector of it's transform.)
phy_vect cpBodyGetRotation(const phy_body *body);

/// Get the user data pointer assigned to the body.
phy_data_pointer cpBodyGetUserData(const phy_body *body);
/// Set the user data pointer assigned to the body.
void cpBodySetUserData(phy_body *body, phy_data_pointer userData);

/// Set the callback used to update a body's velocity.
void cpBodySetVelocityUpdateFunc(phy_body *body, phy_body_velocity_func velocityFunc);
/// Set the callback used to update a body's position.
/// NOTE: It's not generally recommended to override this unless you call the default position update function.
void cpBodySetPositionUpdateFunc(phy_body *body, phy_body_position_func positionFunc);

/// Default velocity integration function..
void cpBodyUpdateVelocity(phy_body *body, phy_vect gravity, float damping, float dt);
/// Default position integration function.
void cpBodyUpdatePosition(phy_body *body, float dt);

/// Convert body relative/local coordinates to absolute/world coordinates.
phy_vect cpBodyLocalToWorld(const phy_body *body, const phy_vect point);
/// Convert body absolute/world coordinates to  relative/local coordinates.
phy_vect cpBodyWorldToLocal(const phy_body *body, const phy_vect point);

/// Apply a force to a body. Both the force and point are expressed in world coordinates.
void cpBodyApplyForceAtWorldPoint(phy_body *body, phy_vect force, phy_vect point);
/// Apply a force to a body. Both the force and point are expressed in body local coordinates.
void cpBodyApplyForceAtLocalPoint(phy_body *body, phy_vect force, phy_vect point);

/// Apply an impulse to a body. Both the impulse and point are expressed in world coordinates.
void cpBodyApplyImpulseAtWorldPoint(phy_body *body, phy_vect impulse, phy_vect point);
/// Apply an impulse to a body. Both the impulse and point are expressed in body local coordinates.
void cpBodyApplyImpulseAtLocalPoint(phy_body *body, phy_vect impulse, phy_vect point);

/// Get the velocity on a body (in world units) at a point on the body in world coordinates.
phy_vect cpBodyGetVelocityAtWorldPoint(const phy_body *body, phy_vect point);
/// Get the velocity on a body (in world units) at a point on the body in local coordinates.
phy_vect cpBodyGetVelocityAtLocalPoint(const phy_body *body, phy_vect point);

/// Get the amount of kinetic energy contained by the body.
float cpBodyKineticEnergy(const phy_body *body);

/// Body/shape iterator callback function type. 
typedef void (*cpBodyShapeIteratorFunc)(phy_body *body, phy_shape *shape, void *data);
/// Call @c func once for each shape attached to @c body and added to the space.
void cpBodyEachShape(phy_body *body, cpBodyShapeIteratorFunc func, void *data);

/// Body/constraint iterator callback function type. 
typedef void (*cpBodyConstraintIteratorFunc)(phy_body *body, phy_constraint *constraint, void *data);
/// Call @c func once for each constraint attached to @c body and added to the space.
void cpBodyEachConstraint(phy_body *body, cpBodyConstraintIteratorFunc func, void *data);

/// Body/arbiter iterator callback function type. 
typedef void (*cpBodyArbiterIteratorFunc)(phy_body *body, phy_arbiter *arbiter, void *data);
/// Call @c func once for each arbiter that is currently active on the body.
void cpBodyEachArbiter(phy_body *body, cpBodyArbiterIteratorFunc func, void *data);

///@}
