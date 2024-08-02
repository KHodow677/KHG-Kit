#pragma once

#include <stdint.h>
#include <float.h>

typedef float cpFloat;
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

#define CP_PI ((cpFloat)3.14159265358979323846264338327950288)

static inline cpFloat cpfmax(cpFloat a, cpFloat b) {
	return (a > b) ? a : b;
}

static inline cpFloat cpfmin(cpFloat a, cpFloat b) {
	return (a < b) ? a : b;
}

static inline cpFloat cpfabs(cpFloat f) {
	return (f < 0) ? -f : f;
}

static inline cpFloat cpfclamp(cpFloat f, cpFloat min, cpFloat max) {
	return cpfmin(cpfmax(f, min), max);
}

static inline cpFloat cpfclamp01(cpFloat f) {
	return cpfmax(0.0f, cpfmin(f, 1.0f));
}

static inline cpFloat cpflerp(cpFloat f1, cpFloat f2, cpFloat t) {
	return f1*(1.0f - t) + f2*t;
}

static inline cpFloat cpflerpconst(cpFloat f1, cpFloat f2, cpFloat d) {
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
typedef struct cpVect{ cpFloat x,y; } cpVect;

typedef struct cpTransform {
  cpFloat a, b, c, d, tx, ty;
} cpTransform;

typedef struct cpMat2x2 {
	cpFloat a, b, c, d;
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

