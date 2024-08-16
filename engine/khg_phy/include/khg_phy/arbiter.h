#pragma once

#include "khg_phy/phy_types.h"
#include <stdbool.h>

#define PHY_MAX_CONTACTS_PER_ARBITER 2

extern float phy_arbiter_get_restitution(const phy_arbiter *arb);
extern void phy_arbiter_set_restitution(phy_arbiter *arb, float restitution);

extern float phy_arbiter_get_friction(const phy_arbiter *arb);
extern void phy_arbiter_set_friction(phy_arbiter *arb, float friction);

extern phy_vect phy_arbiter_get_surface_velocity(phy_arbiter *arb);
extern void phy_arbiter_set_surface_velocity(phy_arbiter *arb, phy_vect vr);

extern phy_data_pointer phy_arbiter_get_user_data(const phy_arbiter *arb);
extern void phy_arbiter_set_user_data(phy_arbiter *arb, phy_data_pointer userData);

extern phy_vect phy_arbiter_total_impulse(const phy_arbiter *arb);
extern float phy_arbiter_total_ke(const phy_arbiter *arb);

extern bool phy_arbiter_ignore(phy_arbiter *arb);

extern void phy_arbiter_get_shapes(const phy_arbiter *arb, phy_shape **a, phy_shape **b);
#define PHY_ARBITER_GET_SHAPES(__arb__, __a__, __b__) phy_shape *__a__, *__b__; phy_arbiter_get_shapes(__arb__, &__a__, &__b__);
extern void phy_arbiter_get_bodies(const phy_arbiter *arb, phy_body **a, phy_body **b);
#define PHY_ARBITER_GET_BODIES(__arb__, __a__, __b__) phy_body *__a__, *__b__; phy_arbiter_get_bodies(__arb__, &__a__, &__b__);

struct phy_contact_point_set {
	int count;
	phy_vect normal;
	struct {
		phy_vect pointA, pointB;
		float distance;
	} points[PHY_MAX_CONTACTS_PER_ARBITER];
};

extern phy_contact_point_set cpArbiterGetContactPointSet(const phy_arbiter *arb);
extern void cpArbiterSetContactPointSet(phy_arbiter *arb, phy_contact_point_set *set);

extern bool cpArbiterIsFirstContact(const phy_arbiter *arb);
extern bool cpArbiterIsRemoval(const phy_arbiter *arb);

extern int cpArbiterGetCount(const phy_arbiter *arb);
extern phy_vect cpArbiterGetNormal(const phy_arbiter *arb);
extern phy_vect cpArbiterGetPointA(const phy_arbiter *arb, int i);
extern phy_vect cpArbiterGetPointB(const phy_arbiter *arb, int i);
extern float cpArbiterGetDepth(const phy_arbiter *arb, int i);

extern bool cpArbiterCallWildcardBeginA(phy_arbiter *arb, phy_space *space);
extern bool cpArbiterCallWildcardBeginB(phy_arbiter *arb, phy_space *space);

extern bool cpArbiterCallWildcardPreSolveA(phy_arbiter *arb, phy_space *space);
extern bool cpArbiterCallWildcardPreSolveB(phy_arbiter *arb, phy_space *space);

extern void cpArbiterCallWildcardPostSolveA(phy_arbiter *arb, phy_space *space);
extern void cpArbiterCallWildcardPostSolveB(phy_arbiter *arb, phy_space *space);

extern void cpArbiterCallWildcardSeparateA(phy_arbiter *arb, phy_space *space);
extern void cpArbiterCallWildcardSeparateB(phy_arbiter *arb, phy_space *space);
