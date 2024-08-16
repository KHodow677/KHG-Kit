#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
bool cpConstraintIsDampedRotarySpring(const phy_constraint *constraint);

/// Function type used for damped rotary spring force callbacks.
typedef float (*cpDampedRotarySpringTorqueFunc)(struct phy_constraint *spring, float relativeAngle);

/// Allocate a damped rotary spring.
phy_damped_rotary_spring* cpDampedRotarySpringAlloc(void);
/// Initialize a damped rotary spring.
phy_damped_rotary_spring* cpDampedRotarySpringInit(phy_damped_rotary_spring *joint, phy_body *a, phy_body *b, float restAngle, float stiffness, float damping);
/// Allocate and initialize a damped rotary spring.
phy_constraint* cpDampedRotarySpringNew(phy_body *a, phy_body *b, float restAngle, float stiffness, float damping);

/// Get the rest length of the spring.
float cpDampedRotarySpringGetRestAngle(const phy_constraint *constraint);
/// Set the rest length of the spring.
void cpDampedRotarySpringSetRestAngle(phy_constraint *constraint, float restAngle);

/// Get the stiffness of the spring in force/distance.
float cpDampedRotarySpringGetStiffness(const phy_constraint *constraint);
/// Set the stiffness of the spring in force/distance.
void cpDampedRotarySpringSetStiffness(phy_constraint *constraint, float stiffness);

/// Get the damping of the spring.
float cpDampedRotarySpringGetDamping(const phy_constraint *constraint);
/// Set the damping of the spring.
void cpDampedRotarySpringSetDamping(phy_constraint *constraint, float damping);

/// Get the damping of the spring.
cpDampedRotarySpringTorqueFunc cpDampedRotarySpringGetSpringTorqueFunc(const phy_constraint *constraint);
/// Set the damping of the spring.
void cpDampedRotarySpringSetSpringTorqueFunc(phy_constraint *constraint, cpDampedRotarySpringTorqueFunc springTorqueFunc);

/// @}
