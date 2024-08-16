#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/pin_joint.h"
#include "khg_phy/slide_joint.h"
#include "khg_phy/pivot_joint.h"
#include "khg_phy/groove_joint.h"
#include "khg_phy/damped_spring.h"
#include "khg_phy/damped_rotary_spring.h"
#include "khg_phy/rotary_limit_joint.h"
#include "khg_phy/ratchet_joint.h"
#include "khg_phy/gear_joint.h"
#include "khg_phy/simple_motor.h"

/// Callback function type that gets called before solving a joint.
typedef void (*cpConstraintPreSolveFunc)(phy_constraint *constraint, phy_space *space);
/// Callback function type that gets called after solving a joint.
typedef void (*cpConstraintPostSolveFunc)(phy_constraint *constraint, phy_space *space);

/// Destroy a constraint.
CP_EXPORT void cpConstraintDestroy(phy_constraint *constraint);
/// Destroy and free a constraint.
CP_EXPORT void cpConstraintFree(phy_constraint *constraint);

/// Get the cpSpace this constraint is added to.
CP_EXPORT phy_space* cpConstraintGetSpace(const phy_constraint *constraint);

/// Get the first body the constraint is attached to.
CP_EXPORT phy_body* cpConstraintGetBodyA(const phy_constraint *constraint);

/// Get the second body the constraint is attached to.
CP_EXPORT phy_body* cpConstraintGetBodyB(const phy_constraint *constraint);

/// Get the maximum force that this constraint is allowed to use.
CP_EXPORT float cpConstraintGetMaxForce(const phy_constraint *constraint);
/// Set the maximum force that this constraint is allowed to use. (defaults to INFINITY)
CP_EXPORT void cpConstraintSetMaxForce(phy_constraint *constraint, float maxForce);

/// Get rate at which joint error is corrected.
CP_EXPORT float cpConstraintGetErrorBias(const phy_constraint *constraint);
/// Set rate at which joint error is corrected.
/// Defaults to pow(1.0 - 0.1, 60.0) meaning that it will
/// correct 10% of the error every 1/60th of a second.
CP_EXPORT void cpConstraintSetErrorBias(phy_constraint *constraint, float errorBias);

/// Get the maximum rate at which joint error is corrected.
CP_EXPORT float cpConstraintGetMaxBias(const phy_constraint *constraint);
/// Set the maximum rate at which joint error is corrected. (defaults to INFINITY)
CP_EXPORT void cpConstraintSetMaxBias(phy_constraint *constraint, float maxBias);

/// Get if the two bodies connected by the constraint are allowed to collide or not.
CP_EXPORT bool cpConstraintGetCollideBodies(const phy_constraint *constraint);
/// Set if the two bodies connected by the constraint are allowed to collide or not. (defaults to cpFalse)
CP_EXPORT void cpConstraintSetCollideBodies(phy_constraint *constraint, bool collideBodies);

/// Get the pre-solve function that is called before the solver runs.
CP_EXPORT cpConstraintPreSolveFunc cpConstraintGetPreSolveFunc(const phy_constraint *constraint);
/// Set the pre-solve function that is called before the solver runs.
CP_EXPORT void cpConstraintSetPreSolveFunc(phy_constraint *constraint, cpConstraintPreSolveFunc preSolveFunc);

/// Get the post-solve function that is called before the solver runs.
CP_EXPORT cpConstraintPostSolveFunc cpConstraintGetPostSolveFunc(const phy_constraint *constraint);
/// Set the post-solve function that is called before the solver runs.
CP_EXPORT void cpConstraintSetPostSolveFunc(phy_constraint *constraint, cpConstraintPostSolveFunc postSolveFunc);

/// Get the user definable data pointer for this constraint
CP_EXPORT phy_data_pointer cpConstraintGetUserData(const phy_constraint *constraint);
/// Set the user definable data pointer for this constraint
CP_EXPORT void cpConstraintSetUserData(phy_constraint *constraint, phy_data_pointer userData);

/// Get the last impulse applied by this constraint.
CP_EXPORT float cpConstraintGetImpulse(phy_constraint *constraint);
