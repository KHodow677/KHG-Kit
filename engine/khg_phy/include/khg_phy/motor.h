#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_motor(const phy_constraint *constraint);

phy_simple_motor_joint *phy_motor_alloc(void);
phy_constraint *phy_motor_new(phy_body *a, phy_body *b, float rate);
phy_simple_motor_joint *phy_motor_init(phy_simple_motor_joint *joint, phy_body *a, phy_body *b, float rate);

float phy_motor_get_rate(const phy_constraint *constraint);
void phy_motor_set_rate(phy_constraint *constraint, float rate);

