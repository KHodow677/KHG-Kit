#pragma once

#include "khg_phy/phy_types.h"

/// Opaque struct type for damped rotary springs.
typedef struct cpSimpleMotor cpSimpleMotor;

/// Check if a constraint is a damped rotary springs.
CP_EXPORT bool cpConstraintIsSimpleMotor(const cpConstraint *constraint);

/// Allocate a simple motor.
CP_EXPORT cpSimpleMotor* cpSimpleMotorAlloc(void);
/// initialize a simple motor.
CP_EXPORT cpSimpleMotor* cpSimpleMotorInit(cpSimpleMotor *joint, cpBody *a, cpBody *b, float rate);
/// Allocate and initialize a simple motor.
CP_EXPORT cpConstraint* cpSimpleMotorNew(cpBody *a, cpBody *b, float rate);

/// Get the rate of the motor.
CP_EXPORT float cpSimpleMotorGetRate(const cpConstraint *constraint);
/// Set the rate of the motor.
CP_EXPORT void cpSimpleMotorSetRate(cpConstraint *constraint, float rate);
