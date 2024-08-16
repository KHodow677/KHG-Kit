#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
bool cpConstraintIsRotaryLimitJoint(const phy_constraint *constraint);

/// Allocate a damped rotary limit joint.
rotary_limit_joint* cpRotaryLimitJointAlloc(void);
/// Initialize a damped rotary limit joint.
rotary_limit_joint* cpRotaryLimitJointInit(rotary_limit_joint *joint, phy_body *a, phy_body *b, float min, float max);
/// Allocate and initialize a damped rotary limit joint.
phy_constraint* cpRotaryLimitJointNew(phy_body *a, phy_body *b, float min, float max);

/// Get the minimum distance the joint will maintain between the two anchors.
float cpRotaryLimitJointGetMin(const phy_constraint *constraint);
/// Set the minimum distance the joint will maintain between the two anchors.
void cpRotaryLimitJointSetMin(phy_constraint *constraint, float min);

/// Get the maximum distance the joint will maintain between the two anchors.
float cpRotaryLimitJointGetMax(const phy_constraint *constraint);
/// Set the maximum distance the joint will maintain between the two anchors.
void cpRotaryLimitJointSetMax(phy_constraint *constraint, float max);

/// @}
