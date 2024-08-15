#pragma once

#include "khg_phy/phy_types.h"
#include <stdbool.h>

#define PHY_MAX_CONTACTS_PER_ARBITER 2

extern float phy_arbiter_get_restitution(const cpArbiter *arb);
extern void phy_arbiter_set_restitution(cpArbiter *arb, float restitution);

extern float phy_arbiter_get_friction(const cpArbiter *arb);
extern void phy_arbiter_set_friction(cpArbiter *arb, float friction);

extern cpVect phy_arbiter_get_surface_velocity(cpArbiter *arb);
extern void phy_arbiter_set_surface_velocity(cpArbiter *arb, cpVect vr);

extern cpDataPointer phy_arbiter_get_user_data(const cpArbiter *arb);
extern void phy_arbiter_set_user_data(cpArbiter *arb, cpDataPointer userData);

extern cpVect phy_arbiter_total_impulse(const cpArbiter *arb);
extern float phy_arbiter_total_ke(const cpArbiter *arb);

extern bool phy_arbiter_ignore(cpArbiter *arb);

extern void phy_arbiter_get_shapes(const cpArbiter *arb, cpShape **a, cpShape **b);
#define PHY_ARBITER_GET_SHAPES(__arb__, __a__, __b__) cpShape *__a__, *__b__; phy_arbiter_get_shapes(__arb__, &__a__, &__b__);
extern void phy_arbiter_get_bodies(const cpArbiter *arb, cpBody **a, cpBody **b);
#define PHY_ARBITER_GET_BODIES(__arb__, __a__, __b__) cpBody *__a__, *__b__; phy_arbiter_get_bodies(__arb__, &__a__, &__b__);

struct phy_contact_point_set {
	int count;
	cpVect normal;
	struct {
		cpVect pointA, pointB;
		float distance;
	} points[PHY_MAX_CONTACTS_PER_ARBITER];
};

extern phy_contact_point_set cpArbiterGetContactPointSet(const cpArbiter *arb);
extern void cpArbiterSetContactPointSet(cpArbiter *arb, phy_contact_point_set *set);

extern bool cpArbiterIsFirstContact(const cpArbiter *arb);
extern bool cpArbiterIsRemoval(const cpArbiter *arb);

extern int cpArbiterGetCount(const cpArbiter *arb);
extern cpVect cpArbiterGetNormal(const cpArbiter *arb);
extern cpVect cpArbiterGetPointA(const cpArbiter *arb, int i);
extern cpVect cpArbiterGetPointB(const cpArbiter *arb, int i);
extern float cpArbiterGetDepth(const cpArbiter *arb, int i);

extern bool cpArbiterCallWildcardBeginA(cpArbiter *arb, cpSpace *space);
extern bool cpArbiterCallWildcardBeginB(cpArbiter *arb, cpSpace *space);

extern bool cpArbiterCallWildcardPreSolveA(cpArbiter *arb, cpSpace *space);
extern bool cpArbiterCallWildcardPreSolveB(cpArbiter *arb, cpSpace *space);

extern void cpArbiterCallWildcardPostSolveA(cpArbiter *arb, cpSpace *space);
extern void cpArbiterCallWildcardPostSolveB(cpArbiter *arb, cpSpace *space);

extern void cpArbiterCallWildcardSeparateA(cpArbiter *arb, cpSpace *space);
extern void cpArbiterCallWildcardSeparateB(cpArbiter *arb, cpSpace *space);
