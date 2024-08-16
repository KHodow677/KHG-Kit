#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a slide joint.
CP_EXPORT bool cpConstraintIsSlideJoint(const phy_constraint *constraint);

/// Allocate a slide joint.
CP_EXPORT phy_slide_joint* cpSlideJointAlloc(void);
/// Initialize a slide joint.
CP_EXPORT phy_slide_joint* cpSlideJointInit(phy_slide_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max);
/// Allocate and initialize a slide joint.
CP_EXPORT phy_constraint* cpSlideJointNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max);

/// Get the location of the first anchor relative to the first body.
CP_EXPORT phy_vect cpSlideJointGetAnchorA(const phy_constraint *constraint);
/// Set the location of the first anchor relative to the first body.
CP_EXPORT void cpSlideJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA);

/// Get the location of the second anchor relative to the second body.
CP_EXPORT phy_vect cpSlideJointGetAnchorB(const phy_constraint *constraint);
/// Set the location of the second anchor relative to the second body.
CP_EXPORT void cpSlideJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB);

/// Get the minimum distance the joint will maintain between the two anchors.
CP_EXPORT float cpSlideJointGetMin(const phy_constraint *constraint);
/// Set the minimum distance the joint will maintain between the two anchors.
CP_EXPORT void cpSlideJointSetMin(phy_constraint *constraint, float min);

/// Get the maximum distance the joint will maintain between the two anchors.
CP_EXPORT float cpSlideJointGetMax(const phy_constraint *constraint);
/// Set the maximum distance the joint will maintain between the two anchors.
CP_EXPORT void cpSlideJointSetMax(phy_constraint *constraint, float max);
