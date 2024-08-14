#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a slide joint.
CP_EXPORT cpBool cpConstraintIsDampedSpring(const cpConstraint *constraint);

/// Function type used for damped spring force callbacks.
typedef float (*cpDampedSpringForceFunc)(cpConstraint *spring, float dist);

/// Allocate a damped spring.
CP_EXPORT cpDampedSpring* cpDampedSpringAlloc(void);
/// Initialize a damped spring.
CP_EXPORT cpDampedSpring* cpDampedSpringInit(cpDampedSpring *joint, cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, float restLength, float stiffness, float damping);
/// Allocate and initialize a damped spring.
CP_EXPORT cpConstraint* cpDampedSpringNew(cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, float restLength, float stiffness, float damping);

/// Get the location of the first anchor relative to the first body.
CP_EXPORT cpVect cpDampedSpringGetAnchorA(const cpConstraint *constraint);
/// Set the location of the first anchor relative to the first body.
CP_EXPORT void cpDampedSpringSetAnchorA(cpConstraint *constraint, cpVect anchorA);

/// Get the location of the second anchor relative to the second body.
CP_EXPORT cpVect cpDampedSpringGetAnchorB(const cpConstraint *constraint);
/// Set the location of the second anchor relative to the second body.
CP_EXPORT void cpDampedSpringSetAnchorB(cpConstraint *constraint, cpVect anchorB);

/// Get the rest length of the spring.
CP_EXPORT float cpDampedSpringGetRestLength(const cpConstraint *constraint);
/// Set the rest length of the spring.
CP_EXPORT void cpDampedSpringSetRestLength(cpConstraint *constraint, float restLength);

/// Get the stiffness of the spring in force/distance.
CP_EXPORT float cpDampedSpringGetStiffness(const cpConstraint *constraint);
/// Set the stiffness of the spring in force/distance.
CP_EXPORT void cpDampedSpringSetStiffness(cpConstraint *constraint, float stiffness);

/// Get the damping of the spring.
CP_EXPORT float cpDampedSpringGetDamping(const cpConstraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedSpringSetDamping(cpConstraint *constraint, float damping);

/// Get the damping of the spring.
CP_EXPORT cpDampedSpringForceFunc cpDampedSpringGetSpringForceFunc(const cpConstraint *constraint);
/// Set the damping of the spring.
CP_EXPORT void cpDampedSpringSetSpringForceFunc(cpConstraint *constraint, cpDampedSpringForceFunc springForceFunc);
