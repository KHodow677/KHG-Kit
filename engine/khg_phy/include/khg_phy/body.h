#pragma once

#include "khg_phy/phy_types.h"

typedef enum cpBodyType {
	/// A dynamic body is one that is affected by gravity, forces, and collisions.
	/// This is the default body type.
	CP_BODY_TYPE_DYNAMIC,
	/// A kinematic body is an infinite mass, user controlled body that is not affected by gravity, forces or collisions.
	/// Instead the body only moves based on it's velocity.
	/// Dynamic bodies collide normally with kinematic bodies, though the kinematic body will be unaffected.
	/// Collisions between two kinematic bodies, or a kinematic body and a static body produce collision callbacks, but no collision response.
	CP_BODY_TYPE_KINEMATIC,
	/// A static body is a body that never (or rarely) moves. If you move a static body, you must call one of the cpSpaceReindex*() functions.
	/// Chipmunk uses this information to optimize the collision detection.
	/// Static bodies do not produce collision callbacks when colliding with other static bodies.
	CP_BODY_TYPE_STATIC,
} cpBodyType;

/// Rigid body velocity update function type.
typedef void (*cpBodyVelocityFunc)(phy_body *body, phy_vect gravity, float damping, float dt);
/// Rigid body position update function type.
typedef void (*cpBodyPositionFunc)(phy_body *body, float dt);

/// Allocate a cpBody.
CP_EXPORT phy_body* cpBodyAlloc(void);
/// Initialize a cpBody.
CP_EXPORT phy_body* cpBodyInit(phy_body *body, float mass, float moment);
/// Allocate and initialize a cpBody.
CP_EXPORT phy_body* cpBodyNew(float mass, float moment);

/// Allocate and initialize a cpBody, and set it as a kinematic body.
CP_EXPORT phy_body* cpBodyNewKinematic(void);
/// Allocate and initialize a cpBody, and set it as a static body.
CP_EXPORT phy_body* cpBodyNewStatic(void);

/// Destroy a cpBody.
CP_EXPORT void cpBodyDestroy(phy_body *body);
/// Destroy and free a cpBody.
CP_EXPORT void cpBodyFree(phy_body *body);

// Defined in cpSpace.c
/// Wake up a sleeping or idle body.
CP_EXPORT void cpBodyActivate(phy_body *body);
/// Wake up any sleeping or idle bodies touching a static body.
CP_EXPORT void cpBodyActivateStatic(phy_body *body, phy_shape *filter);

/// Force a body to fall asleep immediately.
CP_EXPORT void cpBodySleep(phy_body *body);
/// Force a body to fall asleep immediately along with other bodies in a group.
CP_EXPORT void cpBodySleepWithGroup(phy_body *body, phy_body *group);

/// Returns true if the body is sleeping.
CP_EXPORT bool cpBodyIsSleeping(const phy_body *body);

/// Get the type of the body.
CP_EXPORT cpBodyType cpBodyGetType(phy_body *body);
/// Set the type of the body.
CP_EXPORT void cpBodySetType(phy_body *body, cpBodyType type);

/// Get the space this body is added to.
CP_EXPORT phy_space* cpBodyGetSpace(const phy_body *body);

/// Get the mass of the body.
CP_EXPORT float cpBodyGetMass(const phy_body *body);
/// Set the mass of the body.
CP_EXPORT void cpBodySetMass(phy_body *body, float m);

/// Get the moment of inertia of the body.
CP_EXPORT float cpBodyGetMoment(const phy_body *body);
/// Set the moment of inertia of the body.
CP_EXPORT void cpBodySetMoment(phy_body *body, float i);

/// Set the position of a body.
CP_EXPORT phy_vect cpBodyGetPosition(const phy_body *body);
/// Set the position of the body.
CP_EXPORT void cpBodySetPosition(phy_body *body, phy_vect pos);

/// Get the offset of the center of gravity in body local coordinates.
CP_EXPORT phy_vect cpBodyGetCenterOfGravity(const phy_body *body);
/// Set the offset of the center of gravity in body local coordinates.
CP_EXPORT void cpBodySetCenterOfGravity(phy_body *body, phy_vect cog);

/// Get the velocity of the body.
CP_EXPORT phy_vect cpBodyGetVelocity(const phy_body *body);
/// Set the velocity of the body.
CP_EXPORT void cpBodySetVelocity(phy_body *body, phy_vect velocity);

/// Get the force applied to the body for the next time step.
CP_EXPORT phy_vect cpBodyGetForce(const phy_body *body);
/// Set the force applied to the body for the next time step.
CP_EXPORT void cpBodySetForce(phy_body *body, phy_vect force);

/// Get the angle of the body.
CP_EXPORT float cpBodyGetAngle(const phy_body *body);
/// Set the angle of a body.
CP_EXPORT void cpBodySetAngle(phy_body *body, float a);

/// Get the angular velocity of the body.
CP_EXPORT float cpBodyGetAngularVelocity(const phy_body *body);
/// Set the angular velocity of the body.
CP_EXPORT void cpBodySetAngularVelocity(phy_body *body, float angularVelocity);

/// Get the torque applied to the body for the next time step.
CP_EXPORT float cpBodyGetTorque(const phy_body *body);
/// Set the torque applied to the body for the next time step.
CP_EXPORT void cpBodySetTorque(phy_body *body, float torque);

/// Get the rotation vector of the body. (The x basis vector of it's transform.)
CP_EXPORT phy_vect cpBodyGetRotation(const phy_body *body);

/// Get the user data pointer assigned to the body.
CP_EXPORT phy_data_pointer cpBodyGetUserData(const phy_body *body);
/// Set the user data pointer assigned to the body.
CP_EXPORT void cpBodySetUserData(phy_body *body, phy_data_pointer userData);

/// Set the callback used to update a body's velocity.
CP_EXPORT void cpBodySetVelocityUpdateFunc(phy_body *body, cpBodyVelocityFunc velocityFunc);
/// Set the callback used to update a body's position.
/// NOTE: It's not generally recommended to override this unless you call the default position update function.
CP_EXPORT void cpBodySetPositionUpdateFunc(phy_body *body, cpBodyPositionFunc positionFunc);

/// Default velocity integration function..
CP_EXPORT void cpBodyUpdateVelocity(phy_body *body, phy_vect gravity, float damping, float dt);
/// Default position integration function.
CP_EXPORT void cpBodyUpdatePosition(phy_body *body, float dt);

/// Convert body relative/local coordinates to absolute/world coordinates.
CP_EXPORT phy_vect cpBodyLocalToWorld(const phy_body *body, const phy_vect point);
/// Convert body absolute/world coordinates to  relative/local coordinates.
CP_EXPORT phy_vect cpBodyWorldToLocal(const phy_body *body, const phy_vect point);

/// Apply a force to a body. Both the force and point are expressed in world coordinates.
CP_EXPORT void cpBodyApplyForceAtWorldPoint(phy_body *body, phy_vect force, phy_vect point);
/// Apply a force to a body. Both the force and point are expressed in body local coordinates.
CP_EXPORT void cpBodyApplyForceAtLocalPoint(phy_body *body, phy_vect force, phy_vect point);

/// Apply an impulse to a body. Both the impulse and point are expressed in world coordinates.
CP_EXPORT void cpBodyApplyImpulseAtWorldPoint(phy_body *body, phy_vect impulse, phy_vect point);
/// Apply an impulse to a body. Both the impulse and point are expressed in body local coordinates.
CP_EXPORT void cpBodyApplyImpulseAtLocalPoint(phy_body *body, phy_vect impulse, phy_vect point);

/// Get the velocity on a body (in world units) at a point on the body in world coordinates.
CP_EXPORT phy_vect cpBodyGetVelocityAtWorldPoint(const phy_body *body, phy_vect point);
/// Get the velocity on a body (in world units) at a point on the body in local coordinates.
CP_EXPORT phy_vect cpBodyGetVelocityAtLocalPoint(const phy_body *body, phy_vect point);

/// Get the amount of kinetic energy contained by the body.
CP_EXPORT float cpBodyKineticEnergy(const phy_body *body);

/// Body/shape iterator callback function type. 
typedef void (*cpBodyShapeIteratorFunc)(phy_body *body, phy_shape *shape, void *data);
/// Call @c func once for each shape attached to @c body and added to the space.
CP_EXPORT void cpBodyEachShape(phy_body *body, cpBodyShapeIteratorFunc func, void *data);

/// Body/constraint iterator callback function type. 
typedef void (*cpBodyConstraintIteratorFunc)(phy_body *body, phy_constraint *constraint, void *data);
/// Call @c func once for each constraint attached to @c body and added to the space.
CP_EXPORT void cpBodyEachConstraint(phy_body *body, cpBodyConstraintIteratorFunc func, void *data);

/// Body/arbiter iterator callback function type. 
typedef void (*cpBodyArbiterIteratorFunc)(phy_body *body, phy_arbiter *arbiter, void *data);
/// Call @c func once for each arbiter that is currently active on the body.
CP_EXPORT void cpBodyEachArbiter(phy_body *body, cpBodyArbiterIteratorFunc func, void *data);

///@}
