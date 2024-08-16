#pragma once

#include "khg_phy/phy_types.h"

/// Check if a constraint is a damped rotary springs.
bool cpConstraintIsSimpleMotor(const phy_constraint *constraint);

/// Allocate a simple motor.
phy_simple_motor_joint *cpSimpleMotorAlloc(void);
/// initialize a simple motor.
phy_simple_motor_joint *cpSimpleMotorInit(phy_simple_motor_joint *joint, phy_body *a, phy_body *b, float rate);
/// Allocate and initialize a simple motor.
phy_constraint* cpSimpleMotorNew(phy_body *a, phy_body *b, float rate);

/// Get the rate of the motor.
float cpSimpleMotorGetRate(const phy_constraint *constraint);
/// Set the rate of the motor.
void cpSimpleMotorSetRate(phy_constraint *constraint, float rate);
