#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a pin joint.
CP_EXPORT bool cpConstraintIsPinJoint(const phy_constraint *constraint);

/// Allocate a pin joint.
CP_EXPORT phy_pin_joint* cpPinJointAlloc(void);
/// Initialize a pin joint.
CP_EXPORT phy_pin_joint* cpPinJointInit(phy_pin_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB);
/// Allocate and initialize a pin joint.
CP_EXPORT phy_constraint* cpPinJointNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB);

/// Get the location of the first anchor relative to the first body.
CP_EXPORT phy_vect cpPinJointGetAnchorA(const phy_constraint *constraint);
/// Set the location of the first anchor relative to the first body.
CP_EXPORT void cpPinJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA);

/// Get the location of the second anchor relative to the second body.
CP_EXPORT phy_vect cpPinJointGetAnchorB(const phy_constraint *constraint);
/// Set the location of the second anchor relative to the second body.
CP_EXPORT void cpPinJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB);

/// Get the distance the joint will maintain between the two anchors.
CP_EXPORT float cpPinJointGetDist(const phy_constraint *constraint);
/// Set the distance the joint will maintain between the two anchors.
CP_EXPORT void cpPinJointSetDist(phy_constraint *constraint, float dist);

///@}
