#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_pin_joint(const phy_constraint *constraint);

phy_pin_joint *phy_pin_joint_alloc(void);
phy_constraint *phy_pin_joint_new(phy_body *a, phy_body *b, phy_vect anchor_A, phy_vect anchor_B);
phy_pin_joint *phy_pin_joint_init(phy_pin_joint *joint, phy_body *a, phy_body *b, phy_vect anchor_A, phy_vect anchor_B);

phy_vect phy_pin_joint_get_anchor_A(const phy_constraint *constraint);
void phy_pin_joint_set_anchor_A(phy_constraint *constraint, phy_vect anchor_A);

phy_vect phy_pin_joint_get_anchor_B(const phy_constraint *constraint);
void phy_pin_joint_set_anchor_B(phy_constraint *constraint, phy_vect anchor_B);

float phy_pin_joint_get_dist(const phy_constraint *constraint);
void phy_pin_joint_set_dist(phy_constraint *constraint, float dist);

