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
typedef void (*cpBodyVelocityFunc)(cpBody *body, cpVect gravity, float damping, float dt);
/// Rigid body position update function type.
typedef void (*cpBodyPositionFunc)(cpBody *body, float dt);

/// Allocate a cpBody.
CP_EXPORT cpBody* cpBodyAlloc(void);
/// Initialize a cpBody.
CP_EXPORT cpBody* cpBodyInit(cpBody *body, float mass, float moment);
/// Allocate and initialize a cpBody.
CP_EXPORT cpBody* cpBodyNew(float mass, float moment);

/// Allocate and initialize a cpBody, and set it as a kinematic body.
CP_EXPORT cpBody* cpBodyNewKinematic(void);
/// Allocate and initialize a cpBody, and set it as a static body.
CP_EXPORT cpBody* cpBodyNewStatic(void);

/// Destroy a cpBody.
CP_EXPORT void cpBodyDestroy(cpBody *body);
/// Destroy and free a cpBody.
CP_EXPORT void cpBodyFree(cpBody *body);

// Defined in cpSpace.c
/// Wake up a sleeping or idle body.
CP_EXPORT void cpBodyActivate(cpBody *body);
/// Wake up any sleeping or idle bodies touching a static body.
CP_EXPORT void cpBodyActivateStatic(cpBody *body, cpShape *filter);

/// Force a body to fall asleep immediately.
CP_EXPORT void cpBodySleep(cpBody *body);
/// Force a body to fall asleep immediately along with other bodies in a group.
CP_EXPORT void cpBodySleepWithGroup(cpBody *body, cpBody *group);

/// Returns true if the body is sleeping.
CP_EXPORT cpBool cpBodyIsSleeping(const cpBody *body);

/// Get the type of the body.
CP_EXPORT cpBodyType cpBodyGetType(cpBody *body);
/// Set the type of the body.
CP_EXPORT void cpBodySetType(cpBody *body, cpBodyType type);

/// Get the space this body is added to.
CP_EXPORT cpSpace* cpBodyGetSpace(const cpBody *body);

/// Get the mass of the body.
CP_EXPORT float cpBodyGetMass(const cpBody *body);
/// Set the mass of the body.
CP_EXPORT void cpBodySetMass(cpBody *body, float m);

/// Get the moment of inertia of the body.
CP_EXPORT float cpBodyGetMoment(const cpBody *body);
/// Set the moment of inertia of the body.
CP_EXPORT void cpBodySetMoment(cpBody *body, float i);

/// Set the position of a body.
CP_EXPORT cpVect cpBodyGetPosition(const cpBody *body);
/// Set the position of the body.
CP_EXPORT void cpBodySetPosition(cpBody *body, cpVect pos);

/// Get the offset of the center of gravity in body local coordinates.
CP_EXPORT cpVect cpBodyGetCenterOfGravity(const cpBody *body);
/// Set the offset of the center of gravity in body local coordinates.
CP_EXPORT void cpBodySetCenterOfGravity(cpBody *body, cpVect cog);

/// Get the velocity of the body.
CP_EXPORT cpVect cpBodyGetVelocity(const cpBody *body);
/// Set the velocity of the body.
CP_EXPORT void cpBodySetVelocity(cpBody *body, cpVect velocity);

/// Get the force applied to the body for the next time step.
CP_EXPORT cpVect cpBodyGetForce(const cpBody *body);
/// Set the force applied to the body for the next time step.
CP_EXPORT void cpBodySetForce(cpBody *body, cpVect force);

/// Get the angle of the body.
CP_EXPORT float cpBodyGetAngle(const cpBody *body);
/// Set the angle of a body.
CP_EXPORT void cpBodySetAngle(cpBody *body, float a);

/// Get the angular velocity of the body.
CP_EXPORT float cpBodyGetAngularVelocity(const cpBody *body);
/// Set the angular velocity of the body.
CP_EXPORT void cpBodySetAngularVelocity(cpBody *body, float angularVelocity);

/// Get the torque applied to the body for the next time step.
CP_EXPORT float cpBodyGetTorque(const cpBody *body);
/// Set the torque applied to the body for the next time step.
CP_EXPORT void cpBodySetTorque(cpBody *body, float torque);

/// Get the rotation vector of the body. (The x basis vector of it's transform.)
CP_EXPORT cpVect cpBodyGetRotation(const cpBody *body);

/// Get the user data pointer assigned to the body.
CP_EXPORT cpDataPointer cpBodyGetUserData(const cpBody *body);
/// Set the user data pointer assigned to the body.
CP_EXPORT void cpBodySetUserData(cpBody *body, cpDataPointer userData);

/// Set the callback used to update a body's velocity.
CP_EXPORT void cpBodySetVelocityUpdateFunc(cpBody *body, cpBodyVelocityFunc velocityFunc);
/// Set the callback used to update a body's position.
/// NOTE: It's not generally recommended to override this unless you call the default position update function.
CP_EXPORT void cpBodySetPositionUpdateFunc(cpBody *body, cpBodyPositionFunc positionFunc);

/// Default velocity integration function..
CP_EXPORT void cpBodyUpdateVelocity(cpBody *body, cpVect gravity, float damping, float dt);
/// Default position integration function.
CP_EXPORT void cpBodyUpdatePosition(cpBody *body, float dt);

/// Convert body relative/local coordinates to absolute/world coordinates.
CP_EXPORT cpVect cpBodyLocalToWorld(const cpBody *body, const cpVect point);
/// Convert body absolute/world coordinates to  relative/local coordinates.
CP_EXPORT cpVect cpBodyWorldToLocal(const cpBody *body, const cpVect point);

/// Apply a force to a body. Both the force and point are expressed in world coordinates.
CP_EXPORT void cpBodyApplyForceAtWorldPoint(cpBody *body, cpVect force, cpVect point);
/// Apply a force to a body. Both the force and point are expressed in body local coordinates.
CP_EXPORT void cpBodyApplyForceAtLocalPoint(cpBody *body, cpVect force, cpVect point);

/// Apply an impulse to a body. Both the impulse and point are expressed in world coordinates.
CP_EXPORT void cpBodyApplyImpulseAtWorldPoint(cpBody *body, cpVect impulse, cpVect point);
/// Apply an impulse to a body. Both the impulse and point are expressed in body local coordinates.
CP_EXPORT void cpBodyApplyImpulseAtLocalPoint(cpBody *body, cpVect impulse, cpVect point);

/// Get the velocity on a body (in world units) at a point on the body in world coordinates.
CP_EXPORT cpVect cpBodyGetVelocityAtWorldPoint(const cpBody *body, cpVect point);
/// Get the velocity on a body (in world units) at a point on the body in local coordinates.
CP_EXPORT cpVect cpBodyGetVelocityAtLocalPoint(const cpBody *body, cpVect point);

/// Get the amount of kinetic energy contained by the body.
CP_EXPORT float cpBodyKineticEnergy(const cpBody *body);

/// Body/shape iterator callback function type. 
typedef void (*cpBodyShapeIteratorFunc)(cpBody *body, cpShape *shape, void *data);
/// Call @c func once for each shape attached to @c body and added to the space.
CP_EXPORT void cpBodyEachShape(cpBody *body, cpBodyShapeIteratorFunc func, void *data);

/// Body/constraint iterator callback function type. 
typedef void (*cpBodyConstraintIteratorFunc)(cpBody *body, cpConstraint *constraint, void *data);
/// Call @c func once for each constraint attached to @c body and added to the space.
CP_EXPORT void cpBodyEachConstraint(cpBody *body, cpBodyConstraintIteratorFunc func, void *data);

/// Body/arbiter iterator callback function type. 
typedef void (*cpBodyArbiterIteratorFunc)(cpBody *body, cpArbiter *arbiter, void *data);
/// Call @c func once for each arbiter that is currently active on the body.
CP_EXPORT void cpBodyEachArbiter(cpBody *body, cpBodyArbiterIteratorFunc func, void *data);

///@}
