#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a slide joint.
bool cpConstraintIsSlideJoint(const phy_constraint *constraint);

/// Allocate a slide joint.
phy_slide_joint* cpSlideJointAlloc(void);
/// Initialize a slide joint.
phy_slide_joint* cpSlideJointInit(phy_slide_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max);
/// Allocate and initialize a slide joint.
phy_constraint* cpSlideJointNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max);

/// Get the location of the first anchor relative to the first body.
phy_vect cpSlideJointGetAnchorA(const phy_constraint *constraint);
/// Set the location of the first anchor relative to the first body.
void cpSlideJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA);

/// Get the location of the second anchor relative to the second body.
phy_vect cpSlideJointGetAnchorB(const phy_constraint *constraint);
/// Set the location of the second anchor relative to the second body.
void cpSlideJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB);

/// Get the minimum distance the joint will maintain between the two anchors.
float cpSlideJointGetMin(const phy_constraint *constraint);
/// Set the minimum distance the joint will maintain between the two anchors.
void cpSlideJointSetMin(phy_constraint *constraint, float min);

/// Get the maximum distance the joint will maintain between the two anchors.
float cpSlideJointGetMax(const phy_constraint *constraint);
/// Set the maximum distance the joint will maintain between the two anchors.
void cpSlideJointSetMax(phy_constraint *constraint, float max);
