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

extern phy_contact_point_set phy_arbiter_get_contact_point_set(const phy_arbiter *arb);
extern void phy_arbiter_set_contact_point_set(phy_arbiter *arb, phy_contact_point_set *set);

extern bool phy_arbiter_is_first_contact(const phy_arbiter *arb);
extern bool phy_arbiter_is_removal(const phy_arbiter *arb);

extern int phy_arbiter_get_count(const phy_arbiter *arb);
extern phy_vect phy_arbiter_get_normal(const phy_arbiter *arb);
extern phy_vect phy_arbiter_get_point_A(const phy_arbiter *arb, int i);
extern phy_vect phy_arbiter_get_point_B(const phy_arbiter *arb, int i);
extern float phy_arbiter_get_depth(const phy_arbiter *arb, int i);

extern bool phy_arbiter_call_wildcard_begin_A(phy_arbiter *arb, phy_space *space);
extern bool phy_arbiter_call_wildcard_begin_B(phy_arbiter *arb, phy_space *space);

extern bool phy_arbiter_call_wildcard_pre_solve_A(phy_arbiter *arb, phy_space *space);
extern bool phy_arbiter_call_wildcard_pre_solve_B(phy_arbiter *arb, phy_space *space);

extern void phy_arbiter_call_wildcard_post_solve_A(phy_arbiter *arb, phy_space *space);
extern void phy_arbiter_call_wildcard_post_solve_B(phy_arbiter *arb, phy_space *space);

extern void phy_arbiter_call_wildcard_separate_A(phy_arbiter *arb, phy_space *space);
extern void phy_arbiter_call_wildcard_separate_B(phy_arbiter *arb, phy_space *space);

