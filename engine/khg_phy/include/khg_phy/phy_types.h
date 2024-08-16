#pragma once

#include <float.h>
#include <stdbool.h>
#include <stdint.h>

#define CP_PI ((float)3.14159265358979323846264338327950288)

static inline float phy_max(float a, float b) {
	return (a > b) ? a : b;
}

static inline float phy_min(float a, float b) {
	return (a < b) ? a : b;
}

static inline float phy_abs(float f) {
	return (f < 0) ? -f : f;
}

static inline float phy_clamp(float f, float min, float max) {
	return phy_min(phy_max(f, min), max);
}

static inline float phy_clamp01(float f) {
	return phy_max(0.0f, phy_min(f, 1.0f));
}

static inline float phy_lerp(float f1, float f2, float t) {
	return f1*(1.0f - t) + f2*t;
}

static inline float phy_lerpconst(float f1, float f2, float d) {
	return f1 + phy_clamp(f2 - f1, -d, d);
}

typedef uintptr_t phy_hash_value;
typedef uint32_t phy_collision_id;
typedef void *phy_data_pointer;
typedef uintptr_t phy_collision_type;
typedef uintptr_t phy_group;
typedef unsigned int phy_bitmask;
typedef unsigned int phy_timestamp;
#define PHY_NO_GROUP ((phy_group)0)
#define PHY_ALL_CATEGORIES (~(phy_bitmask)0)
#define PHY_WILDCARD_COLLISION_TYPE (~(phy_collision_type)0)

typedef struct phy_vect{ 
  float x, y; 
} phy_vect;

typedef struct phy_transform {
  float a, b, c, d, tx, ty;
} phy_transform;

typedef struct phy_mat2x2 {
	float a, b, c, d;
} phy_mat2x2;

#define PHY_BUFFER_BYTES (32*1024)

typedef struct phy_array phy_array;
typedef struct phy_hash_set phy_hash_set;

typedef struct phy_body phy_body;

typedef struct phy_shape phy_shape;
typedef struct phy_circle_shape phy_circle_shape;
typedef struct phy_segment_shape phy_segment_shape;
typedef struct phy_poly_shape phy_poly_shape;

typedef struct phy_constraint phy_constraint;
typedef struct phy_pin_joint phy_pin_joint;
typedef struct phy_slide_joint phy_slide_joint;
typedef struct phy_pivot_joint phy_pivot_joint;
typedef struct phy_groove_joint phy_groove_joint;
typedef struct phy_damped_spring phy_damped_spring;
typedef struct phy_damped_rotary_spring phy_damped_rotary_spring;
typedef struct rotary_limit_joint rotary_limit_joint;
typedef struct phy_ratchet_joint phy_ratchet_joint;
typedef struct phy_gear_joint phy_gear_joint;
typedef struct phy_simple_motor_joint phy_simple_motor_joint;

typedef struct phy_collision_handler phy_collision_handler;
typedef struct phy_contact_point_set phy_contact_point_set;
typedef struct phy_arbiter phy_arbiter;

typedef struct phy_space phy_space;

#ifndef alloca
#if defined(_WIN32) || defined(_WIN64)
#include <malloc.h>
#else
#include <alloca.h>
#endif
#endif

