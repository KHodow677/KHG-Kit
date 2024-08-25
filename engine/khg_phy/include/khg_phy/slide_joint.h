#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_slide_joint(const phy_constraint *constraint);

phy_slide_joint *phy_slide_joint_alloc(void);
phy_constraint *phy_slide_joint_new(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max);
phy_slide_joint *phy_slide_joint_init(phy_slide_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max);

phy_vect phy_slide_joint_get_anchor_A(const phy_constraint *constraint);
void phy_slide_joint_set_anchor_A(phy_constraint *constraint, phy_vect anchorA);

phy_vect phy_slide_joint_get_anchor_B(const phy_constraint *constraint);
void phy_slide_joint_set_anchor_B(phy_constraint *constraint, phy_vect anchorB);

float phy_slide_joint_get_min(const phy_constraint *constraint);
void phy_slide_joint_set_min(phy_constraint *constraint, float min);

float phy_slide_joint_get_max(const phy_constraint *constraint);
void phy_slide_joint_set_max(phy_constraint *constraint, float max);

