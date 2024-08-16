#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
CP_EXPORT bool cpConstraintIsGearJoint(const phy_constraint *constraint);

/// Allocate a gear joint.
CP_EXPORT phy_gear_joint* cpGearJointAlloc(void);
/// Initialize a gear joint.
CP_EXPORT phy_gear_joint* cpGearJointInit(phy_gear_joint *joint, phy_body *a, phy_body *b, float phase, float ratio);
/// Allocate and initialize a gear joint.
CP_EXPORT phy_constraint* cpGearJointNew(phy_body *a, phy_body *b, float phase, float ratio);

/// Get the phase offset of the gears.
CP_EXPORT float cpGearJointGetPhase(const phy_constraint *constraint);
/// Set the phase offset of the gears.
CP_EXPORT void cpGearJointSetPhase(phy_constraint *constraint, float phase);

/// Get the angular distance of each ratchet.
CP_EXPORT float cpGearJointGetRatio(const phy_constraint *constraint);
/// Set the ratio of a gear joint.
CP_EXPORT void cpGearJointSetRatio(phy_constraint *constraint, float ratio);

/// @}
