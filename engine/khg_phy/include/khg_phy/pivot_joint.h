#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_pivot_joint(const phy_constraint *constraint);

phy_pivot_joint *phy_pivot_joint_alloc(void);
phy_constraint *phy_pivot_joint_new(phy_body *a, phy_body *b, phy_vect pivot);
phy_constraint *phy_pivot_joint_new_2(phy_body *a, phy_body *b, phy_vect anchor_A, phy_vect anchor_B);
phy_pivot_joint *phy_pivot_joint_init(phy_pivot_joint *joint, phy_body *a, phy_body *b, phy_vect anchor_A, phy_vect anchor_B);

phy_vect phy_pivot_joint_get_anchor_A(const phy_constraint *constraint);
void phy_pivot_joint_set_anchor_A(phy_constraint *constraint, phy_vect anchor_A);

phy_vect phy_pivot_joint_get_anchor_B(const phy_constraint *constraint);
void phy_pivot_joint_set_anchor_B(phy_constraint *constraint, phy_vect anchor_B);

