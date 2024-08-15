#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
CP_EXPORT bool cpConstraintIsRotaryLimitJoint(const cpConstraint *constraint);

/// Allocate a damped rotary limit joint.
CP_EXPORT cpRotaryLimitJoint* cpRotaryLimitJointAlloc(void);
/// Initialize a damped rotary limit joint.
CP_EXPORT cpRotaryLimitJoint* cpRotaryLimitJointInit(cpRotaryLimitJoint *joint, cpBody *a, cpBody *b, float min, float max);
/// Allocate and initialize a damped rotary limit joint.
CP_EXPORT cpConstraint* cpRotaryLimitJointNew(cpBody *a, cpBody *b, float min, float max);

/// Get the minimum distance the joint will maintain between the two anchors.
CP_EXPORT float cpRotaryLimitJointGetMin(const cpConstraint *constraint);
/// Set the minimum distance the joint will maintain between the two anchors.
CP_EXPORT void cpRotaryLimitJointSetMin(cpConstraint *constraint, float min);

/// Get the maximum distance the joint will maintain between the two anchors.
CP_EXPORT float cpRotaryLimitJointGetMax(const cpConstraint *constraint);
/// Set the maximum distance the joint will maintain between the two anchors.
CP_EXPORT void cpRotaryLimitJointSetMax(cpConstraint *constraint, float max);

/// @}
