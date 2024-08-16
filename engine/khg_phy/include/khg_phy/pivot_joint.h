#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a slide joint.
CP_EXPORT bool cpConstraintIsPivotJoint(const phy_constraint *constraint);

/// Allocate a pivot joint
CP_EXPORT phy_pivot_joint* cpPivotJointAlloc(void);
/// Initialize a pivot joint.
CP_EXPORT phy_pivot_joint* cpPivotJointInit(phy_pivot_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB);
/// Allocate and initialize a pivot joint.
CP_EXPORT phy_constraint* cpPivotJointNew(phy_body *a, phy_body *b, phy_vect pivot);
/// Allocate and initialize a pivot joint with specific anchors.
CP_EXPORT phy_constraint* cpPivotJointNew2(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB);

/// Get the location of the first anchor relative to the first body.
CP_EXPORT phy_vect cpPivotJointGetAnchorA(const phy_constraint *constraint);
/// Set the location of the first anchor relative to the first body.
CP_EXPORT void cpPivotJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA);

/// Get the location of the second anchor relative to the second body.
CP_EXPORT phy_vect cpPivotJointGetAnchorB(const phy_constraint *constraint);
/// Set the location of the second anchor relative to the second body.
CP_EXPORT void cpPivotJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB);

/// @}
