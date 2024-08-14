#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
CP_EXPORT bool cpConstraintIsGearJoint(const cpConstraint *constraint);

/// Allocate a gear joint.
CP_EXPORT cpGearJoint* cpGearJointAlloc(void);
/// Initialize a gear joint.
CP_EXPORT cpGearJoint* cpGearJointInit(cpGearJoint *joint, cpBody *a, cpBody *b, float phase, float ratio);
/// Allocate and initialize a gear joint.
CP_EXPORT cpConstraint* cpGearJointNew(cpBody *a, cpBody *b, float phase, float ratio);

/// Get the phase offset of the gears.
CP_EXPORT float cpGearJointGetPhase(const cpConstraint *constraint);
/// Set the phase offset of the gears.
CP_EXPORT void cpGearJointSetPhase(cpConstraint *constraint, float phase);

/// Get the angular distance of each ratchet.
CP_EXPORT float cpGearJointGetRatio(const cpConstraint *constraint);
/// Set the ratio of a gear joint.
CP_EXPORT void cpGearJointSetRatio(cpConstraint *constraint, float ratio);

/// @}
