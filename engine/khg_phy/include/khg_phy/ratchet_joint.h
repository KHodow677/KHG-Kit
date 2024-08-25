#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_ratchet_joint(const phy_constraint *constraint);

phy_ratchet_joint *phy_ratchet_joint_alloc(void);
phy_constraint *phy_ratchet_joint_new(phy_body *a, phy_body *b, float phase, float ratchet);
phy_ratchet_joint *phy_ratchet_joint_init(phy_ratchet_joint *joint, phy_body *a, phy_body *b, float phase, float ratchet);

float phy_ratchet_joint_get_angle(const phy_constraint *constraint);
void phy_ratchet_joint_set_angle(phy_constraint *constraint, float angle);

float phy_ratchet_joint_get_phase(const phy_constraint *constraint);
void phy_ratchet_joint_set_phase(phy_constraint *constraint, float phase);

float phy_ratchet_joint_get_ratchet(const phy_constraint *constraint);
void phy_ratchet_joint_set_ratchet(phy_constraint *constraint, float ratchet);

