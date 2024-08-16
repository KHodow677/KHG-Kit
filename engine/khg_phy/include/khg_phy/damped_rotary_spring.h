#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
CP_EXPORT bool cpConstraintIsDampedRotarySpring(const phy_constraint *constraint);

/// Function type used for damped rotary spring force callbacks.
typedef float (*cpDampedRotarySpringTorqueFunc)(struct phy_constraint *spring, float relativeAngle);

/// Allocate a damped rotary spring.
CP_EXPORT phy_damped_rotary_spring* cpDampedRotarySpringAlloc(void);
/// Initialize a damped rotary spring.
CP_EXPORT phy_damped_rotary_spring* cpDampedRotarySpringInit(phy_damped_rotary_spring *joint, phy_body *a, phy_body *b, float restAngle, float stiffness, float damping);
/// Allocate and initialize a damped rotary spring.
CP_EXPORT phy_constraint* cpDampedRotarySpringNew(phy_body *a, phy_body *b, float restAngle, float stiffness, float damping);

/// Get the rest length of the spring.
CP_EXPORT float cpDampedRotarySpringGetRestAngle(const phy_constraint *constraint);
/// Set the rest length of the spring.
CP_EXPORT void cpDampedRotarySpringSetRestAngle(phy_constraint *constraint, float restAngle);

/// Get the stiffness of the spring in force/distance.
CP_EXPORT float cpDampedRotarySpringGetStiffness(const phy_constraint *constraint);
/// Set the stiffness of the spring in force/distance.
CP_EXPORT void cpDampedRotarySpringSetStiffness(phy_constraint *constraint, float stiffness);

/// Get the damping of the spring.
CP_EXPORT float cpDampedRotarySpringGetDamping(const phy_constraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedRotarySpringSetDamping(phy_constraint *constraint, float damping);

/// Get the damping of the spring.
CP_EXPORT cpDampedRotarySpringTorqueFunc cpDampedRotarySpringGetSpringTorqueFunc(const phy_constraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedRotarySpringSetSpringTorqueFunc(phy_constraint *constraint, cpDampedRotarySpringTorqueFunc springTorqueFunc);

/// @}
