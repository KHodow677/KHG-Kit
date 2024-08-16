#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a slide joint.
bool cpConstraintIsGrooveJoint(const phy_constraint *constraint);

/// Allocate a groove joint.
phy_groove_joint* cpGrooveJointAlloc(void);
/// Initialize a groove joint.
phy_groove_joint* cpGrooveJointInit(phy_groove_joint *joint, phy_body *a, phy_body *b, phy_vect groove_a, phy_vect groove_b, phy_vect anchorB);
/// Allocate and initialize a groove joint.
phy_constraint* cpGrooveJointNew(phy_body *a, phy_body *b, phy_vect groove_a, phy_vect groove_b, phy_vect anchorB);

/// Get the first endpoint of the groove relative to the first body.
phy_vect cpGrooveJointGetGrooveA(const phy_constraint *constraint);
/// Set the first endpoint of the groove relative to the first body.
void cpGrooveJointSetGrooveA(phy_constraint *constraint, phy_vect grooveA);

/// Get the first endpoint of the groove relative to the first body.
phy_vect cpGrooveJointGetGrooveB(const phy_constraint *constraint);
/// Set the first endpoint of the groove relative to the first body.
void cpGrooveJointSetGrooveB(phy_constraint *constraint, phy_vect grooveB);

/// Get the location of the second anchor relative to the second body.
phy_vect cpGrooveJointGetAnchorB(const phy_constraint *constraint);
/// Set the location of the second anchor relative to the second body.
void cpGrooveJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB);

/// @}
