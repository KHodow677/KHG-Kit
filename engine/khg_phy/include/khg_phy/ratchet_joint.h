#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
CP_EXPORT cpBool cpConstraintIsRatchetJoint(const cpConstraint *constraint);

/// Allocate a ratchet joint.
CP_EXPORT cpRatchetJoint* cpRatchetJointAlloc(void);
/// Initialize a ratched joint.
CP_EXPORT cpRatchetJoint* cpRatchetJointInit(cpRatchetJoint *joint, cpBody *a, cpBody *b, float phase, float ratchet);
/// Allocate and initialize a ratchet joint.
CP_EXPORT cpConstraint* cpRatchetJointNew(cpBody *a, cpBody *b, float phase, float ratchet);

/// Get the angle of the current ratchet tooth.
CP_EXPORT float cpRatchetJointGetAngle(const cpConstraint *constraint);
/// Set the angle of the current ratchet tooth.
CP_EXPORT void cpRatchetJointSetAngle(cpConstraint *constraint, float angle);

/// Get the phase offset of the ratchet.
CP_EXPORT float cpRatchetJointGetPhase(const cpConstraint *constraint);
/// Get the phase offset of the ratchet.
CP_EXPORT void cpRatchetJointSetPhase(cpConstraint *constraint, float phase);

/// Get the angular distance of each ratchet.
CP_EXPORT float cpRatchetJointGetRatchet(const cpConstraint *constraint);
/// Set the angular distance of each ratchet.
CP_EXPORT void cpRatchetJointSetRatchet(cpConstraint *constraint, float ratchet);

/// @}
