#pragma once

#include "khg_phy/phy_types.h"

#define PHY_MAX_CONTACTS_PER_ARBITER 2

extern float phy_arbiter_get_restitution(const cpArbiter *arb);
extern void phy_arbiter_set_restitution(cpArbiter *arb, float restitution);

extern float phy_arbiter_get_friction(const cpArbiter *arb);
extern void phy_arbiter_set_friction(cpArbiter *arb, float friction);

extern cpVect phy_arbiter_get_surface_velocity(cpArbiter *arb);
extern void phy_arbiter_set_surface_velocity(cpArbiter *arb, cpVect vr);

extern cpDataPointer cpArbiterGetUserData(const cpArbiter *arb);
extern void cpArbiterSetUserData(cpArbiter *arb, cpDataPointer userData);

extern cpVect cpArbiterTotalImpulse(const cpArbiter *arb);
extern float cpArbiterTotalKE(const cpArbiter *arb);

extern cpBool cpArbiterIgnore(cpArbiter *arb);

extern void cpArbiterGetShapes(const cpArbiter *arb, cpShape **a, cpShape **b);
#define CP_ARBITER_GET_SHAPES(__arb__, __a__, __b__) cpShape *__a__, *__b__; cpArbiterGetShapes(__arb__, &__a__, &__b__);
extern void cpArbiterGetBodies(const cpArbiter *arb, cpBody **a, cpBody **b);
#define CP_ARBITER_GET_BODIES(__arb__, __a__, __b__) cpBody *__a__, *__b__; cpArbiterGetBodies(__arb__, &__a__, &__b__);

struct cpContactPointSet {
	int count;
	cpVect normal;
	struct {
		cpVect pointA, pointB;
		float distance;
	} points[PHY_MAX_CONTACTS_PER_ARBITER];
};

extern cpContactPointSet cpArbiterGetContactPointSet(const cpArbiter *arb);
extern void cpArbiterSetContactPointSet(cpArbiter *arb, cpContactPointSet *set);

extern cpBool cpArbiterIsFirstContact(const cpArbiter *arb);
extern cpBool cpArbiterIsRemoval(const cpArbiter *arb);

extern int cpArbiterGetCount(const cpArbiter *arb);
extern cpVect cpArbiterGetNormal(const cpArbiter *arb);
extern cpVect cpArbiterGetPointA(const cpArbiter *arb, int i);
extern cpVect cpArbiterGetPointB(const cpArbiter *arb, int i);
extern float cpArbiterGetDepth(const cpArbiter *arb, int i);

extern cpBool cpArbiterCallWildcardBeginA(cpArbiter *arb, cpSpace *space);
extern cpBool cpArbiterCallWildcardBeginB(cpArbiter *arb, cpSpace *space);

extern cpBool cpArbiterCallWildcardPreSolveA(cpArbiter *arb, cpSpace *space);
extern cpBool cpArbiterCallWildcardPreSolveB(cpArbiter *arb, cpSpace *space);

extern void cpArbiterCallWildcardPostSolveA(cpArbiter *arb, cpSpace *space);
extern void cpArbiterCallWildcardPostSolveB(cpArbiter *arb, cpSpace *space);

extern void cpArbiterCallWildcardSeparateA(cpArbiter *arb, cpSpace *space);
extern void cpArbiterCallWildcardSeparateB(cpArbiter *arb, cpSpace *space);
