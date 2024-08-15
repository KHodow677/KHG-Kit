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
typedef void *cpDataPointer;
typedef uintptr_t cpCollisionType;
typedef uintptr_t cpGroup;
typedef unsigned int cpBitmask;
typedef unsigned int cpTimestamp;
#define CP_NO_GROUP ((cpGroup)0)
#define CP_ALL_CATEGORIES (~(cpBitmask)0)
#define CP_WILDCARD_COLLISION_TYPE (~(cpCollisionType)0)
typedef struct cpVect{ float x,y; } cpVect;

typedef struct cpTransform {
  float a, b, c, d, tx, ty;
} cpTransform;

typedef struct cpMat2x2 {
	float a, b, c, d;
} cpMat2x2;

#define CP_BUFFER_BYTES (32*1024)

#define cpcalloc calloc
#define cprealloc realloc
#define cpfree free

typedef struct cpArray cpArray;
typedef struct cpHashSet cpHashSet;

typedef struct cpBody cpBody;

typedef struct cpShape cpShape;
typedef struct cpCircleShape cpCircleShape;
typedef struct cpSegmentShape cpSegmentShape;
typedef struct cpPolyShape cpPolyShape;

typedef struct cpConstraint cpConstraint;
typedef struct cpPinJoint cpPinJoint;
typedef struct cpSlideJoint cpSlideJoint;
typedef struct cpPivotJoint cpPivotJoint;
typedef struct cpGrooveJoint cpGrooveJoint;
typedef struct cpDampedSpring cpDampedSpring;
typedef struct cpDampedRotarySpring cpDampedRotarySpring;
typedef struct cpRotaryLimitJoint cpRotaryLimitJoint;
typedef struct cpRatchetJoint cpRatchetJoint;
typedef struct cpGearJoint cpGearJoint;
typedef struct cpSimpleMotorJoint cpSimpleMotorJoint;

typedef struct cpCollisionHandler cpCollisionHandler;
typedef struct phy_contact_point_set phy_contact_point_set;
typedef struct cpArbiter cpArbiter;

typedef struct cpSpace cpSpace;

#ifndef alloca
	#ifdef _WIN32
		#include <malloc.h>
	#elif defined(__FreeBSD__)
	#else
		#include <alloca.h>
	#endif
#endif

#ifdef _WIN32
	#define CP_EXPORT __declspec(dllexport)
#else
	#define CP_EXPORT
#endif

