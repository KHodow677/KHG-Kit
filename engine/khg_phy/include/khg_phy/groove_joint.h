#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_groove_joint(const phy_constraint *constraint);

phy_groove_joint* phy_groove_joint_alloc(void);
phy_groove_joint* phy_groove_joint_init(phy_groove_joint *joint, phy_body *a, phy_body *b, phy_vect groove_A, phy_vect groove_B, phy_vect anchor_B);
phy_constraint* phy_groove_joint_new(phy_body *a, phy_body *b, phy_vect groove_A, phy_vect groove_B, phy_vect anchor_B);

phy_vect phy_groove_joint_get_groove_A(const phy_constraint *constraint);
void phy_groove_joint_set_groove_A(phy_constraint *constraint, phy_vect groove_A);

phy_vect phy_groove_joint_get_groove_B(const phy_constraint *constraint);
void phy_groove_joint_set_groove_B(phy_constraint *constraint, phy_vect groove_B);

phy_vect phy_groove_joint_get_anchor_B(const phy_constraint *constraint);
void phy_groove_joint_set_anchor_B(phy_constraint *constraint, phy_vect anchor_B);

