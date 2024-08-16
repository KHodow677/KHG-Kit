#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a slide joint.
CP_EXPORT bool cpConstraintIsDampedSpring(const phy_constraint *constraint);

/// Function type used for damped spring force callbacks.
typedef float (*cpDampedSpringForceFunc)(phy_constraint *spring, float dist);

/// Allocate a damped spring.
CP_EXPORT phy_damped_spring* cpDampedSpringAlloc(void);
/// Initialize a damped spring.
CP_EXPORT phy_damped_spring* cpDampedSpringInit(phy_damped_spring *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping);
/// Allocate and initialize a damped spring.
CP_EXPORT phy_constraint* cpDampedSpringNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping);

/// Get the location of the first anchor relative to the first body.
CP_EXPORT phy_vect cpDampedSpringGetAnchorA(const phy_constraint *constraint);
/// Set the location of the first anchor relative to the first body.
CP_EXPORT void cpDampedSpringSetAnchorA(phy_constraint *constraint, phy_vect anchorA);

/// Get the location of the second anchor relative to the second body.
CP_EXPORT phy_vect cpDampedSpringGetAnchorB(const phy_constraint *constraint);
/// Set the location of the second anchor relative to the second body.
CP_EXPORT void cpDampedSpringSetAnchorB(phy_constraint *constraint, phy_vect anchorB);

/// Get the rest length of the spring.
CP_EXPORT float cpDampedSpringGetRestLength(const phy_constraint *constraint);
/// Set the rest length of the spring.
CP_EXPORT void cpDampedSpringSetRestLength(phy_constraint *constraint, float restLength);

/// Get the stiffness of the spring in force/distance.
CP_EXPORT float cpDampedSpringGetStiffness(const phy_constraint *constraint);
/// Set the stiffness of the spring in force/distance.
CP_EXPORT void cpDampedSpringSetStiffness(phy_constraint *constraint, float stiffness);

/// Get the damping of the spring.
CP_EXPORT float cpDampedSpringGetDamping(const phy_constraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedSpringSetDamping(phy_constraint *constraint, float damping);

/// Get the damping of the spring.
CP_EXPORT cpDampedSpringForceFunc cpDampedSpringGetSpringForceFunc(const phy_constraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedSpringSetSpringForceFunc(phy_constraint *constraint, cpDampedSpringForceFunc springForceFunc);
