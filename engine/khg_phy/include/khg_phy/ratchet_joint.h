#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
bool cpConstraintIsRatchetJoint(const phy_constraint *constraint);

/// Allocate a ratchet joint.
phy_ratchet_joint* cpRatchetJointAlloc(void);
/// Initialize a ratched joint.
phy_ratchet_joint* cpRatchetJointInit(phy_ratchet_joint *joint, phy_body *a, phy_body *b, float phase, float ratchet);
/// Allocate and initialize a ratchet joint.
phy_constraint* cpRatchetJointNew(phy_body *a, phy_body *b, float phase, float ratchet);

/// Get the angle of the current ratchet tooth.
float cpRatchetJointGetAngle(const phy_constraint *constraint);
/// Set the angle of the current ratchet tooth.
void cpRatchetJointSetAngle(phy_constraint *constraint, float angle);

/// Get the phase offset of the ratchet.
float cpRatchetJointGetPhase(const phy_constraint *constraint);
/// Get the phase offset of the ratchet.
void cpRatchetJointSetPhase(phy_constraint *constraint, float phase);

/// Get the angular distance of each ratchet.
float cpRatchetJointGetRatchet(const phy_constraint *constraint);
/// Set the angular distance of each ratchet.
void cpRatchetJointSetRatchet(phy_constraint *constraint, float ratchet);

/// @}
