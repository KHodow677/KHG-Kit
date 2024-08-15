#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
CP_EXPORT bool cpConstraintIsDampedRotarySpring(const cpConstraint *constraint);

/// Function type used for damped rotary spring force callbacks.
typedef float (*cpDampedRotarySpringTorqueFunc)(struct cpConstraint *spring, float relativeAngle);

/// Allocate a damped rotary spring.
CP_EXPORT cpDampedRotarySpring* cpDampedRotarySpringAlloc(void);
/// Initialize a damped rotary spring.
CP_EXPORT cpDampedRotarySpring* cpDampedRotarySpringInit(cpDampedRotarySpring *joint, cpBody *a, cpBody *b, float restAngle, float stiffness, float damping);
/// Allocate and initialize a damped rotary spring.
CP_EXPORT cpConstraint* cpDampedRotarySpringNew(cpBody *a, cpBody *b, float restAngle, float stiffness, float damping);

/// Get the rest length of the spring.
CP_EXPORT float cpDampedRotarySpringGetRestAngle(const cpConstraint *constraint);
/// Set the rest length of the spring.
CP_EXPORT void cpDampedRotarySpringSetRestAngle(cpConstraint *constraint, float restAngle);

/// Get the stiffness of the spring in force/distance.
CP_EXPORT float cpDampedRotarySpringGetStiffness(const cpConstraint *constraint);
/// Set the stiffness of the spring in force/distance.
CP_EXPORT void cpDampedRotarySpringSetStiffness(cpConstraint *constraint, float stiffness);

/// Get the damping of the spring.
CP_EXPORT float cpDampedRotarySpringGetDamping(const cpConstraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedRotarySpringSetDamping(cpConstraint *constraint, float damping);

/// Get the damping of the spring.
CP_EXPORT cpDampedRotarySpringTorqueFunc cpDampedRotarySpringGetSpringTorqueFunc(const cpConstraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedRotarySpringSetSpringTorqueFunc(cpConstraint *constraint, cpDampedRotarySpringTorqueFunc springTorqueFunc);

/// @}
