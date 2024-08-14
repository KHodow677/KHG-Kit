#pragma once

#include <stdint.h>
#include <float.h>

#define cpfsqrt sqrtf
#define cpfsin sinf
#define cpfcos cosf
#define cpfacos acosf
#define cpfatan2 atan2f
#define cpfmod fmodf
#define cpfexp expf
#define cpfpow powf
#define cpffloor floorf
#define cpfceil ceilf
#define CPFLOAT_MIN FLT_MIN

#define CP_PI ((float)3.14159265358979323846264338327950288)

static inline float cpfmax(float a, float b) {
	return (a > b) ? a : b;
}

static inline float cpfmin(float a, float b) {
	return (a < b) ? a : b;
}

static inline float cpfabs(float f) {
	return (f < 0) ? -f : f;
}

static inline float cpfclamp(float f, float min, float max) {
	return cpfmin(cpfmax(f, min), max);
}

static inline float cpfclamp01(float f) {
	return cpfmax(0.0f, cpfmin(f, 1.0f));
}

static inline float cpflerp(float f1, float f2, float t) {
	return f1*(1.0f - t) + f2*t;
}

static inline float cpflerpconst(float f1, float f2, float d) {
	return f1 + cpfclamp(f2 - f1, -d, d);
}

typedef uintptr_t cpHashValue;
typedef uint32_t cpCollisionID;
typedef unsigned char cpBool;
#define cpTrue 1
#define cpFalse 0
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
typedef struct cpContactPointSet cpContactPointSet;
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

