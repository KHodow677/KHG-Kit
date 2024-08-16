#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
CP_EXPORT bool cpConstraintIsRatchetJoint(const phy_constraint *constraint);

/// Allocate a ratchet joint.
CP_EXPORT phy_ratchet_joint* cpRatchetJointAlloc(void);
/// Initialize a ratched joint.
CP_EXPORT phy_ratchet_joint* cpRatchetJointInit(phy_ratchet_joint *joint, phy_body *a, phy_body *b, float phase, float ratchet);
/// Allocate and initialize a ratchet joint.
CP_EXPORT phy_constraint* cpRatchetJointNew(phy_body *a, phy_body *b, float phase, float ratchet);

/// Get the angle of the current ratchet tooth.
CP_EXPORT float cpRatchetJointGetAngle(const phy_constraint *constraint);
/// Set the angle of the current ratchet tooth.
CP_EXPORT void cpRatchetJointSetAngle(phy_constraint *constraint, float angle);

/// Get the phase offset of the ratchet.
CP_EXPORT float cpRatchetJointGetPhase(const phy_constraint *constraint);
/// Get the phase offset of the ratchet.
CP_EXPORT void cpRatchetJointSetPhase(phy_constraint *constraint, float phase);

/// Get the angular distance of each ratchet.
CP_EXPORT float cpRatchetJointGetRatchet(const phy_constraint *constraint);
/// Set the angular distance of each ratchet.
CP_EXPORT void cpRatchetJointSetRatchet(phy_constraint *constraint, float ratchet);

/// @}
