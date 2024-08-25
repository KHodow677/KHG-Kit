#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_rotary_limit_joint(const phy_constraint *constraint);

rotary_limit_joint* phy_rotary_limit_joint_alloc(void);
phy_constraint* phy_rotary_limit_joint_new(phy_body *a, phy_body *b, float min, float max);
rotary_limit_joint* phy_rotary_limit_joint_init(rotary_limit_joint *joint, phy_body *a, phy_body *b, float min, float max);

float phy_rotary_limit_joint_get_min(const phy_constraint *constraint);
void phy_rotary_limit_joint_set_min(phy_constraint *constraint, float min);

float phy_rotary_limit_joint_get_max(const phy_constraint *constraint);
void phy_rotary_limit_joint_set_max(phy_constraint *constraint, float max);

