#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
bool cpConstraintIsGearJoint(const phy_constraint *constraint);

/// Allocate a gear joint.
phy_gear_joint* cpGearJointAlloc(void);
/// Initialize a gear joint.
phy_gear_joint* cpGearJointInit(phy_gear_joint *joint, phy_body *a, phy_body *b, float phase, float ratio);
/// Allocate and initialize a gear joint.
phy_constraint* cpGearJointNew(phy_body *a, phy_body *b, float phase, float ratio);

/// Get the phase offset of the gears.
float cpGearJointGetPhase(const phy_constraint *constraint);
/// Set the phase offset of the gears.
void cpGearJointSetPhase(phy_constraint *constraint, float phase);

/// Get the angular distance of each ratchet.
float cpGearJointGetRatio(const phy_constraint *constraint);
/// Set the ratio of a gear joint.
void cpGearJointSetRatio(phy_constraint *constraint, float ratio);

/// @}
