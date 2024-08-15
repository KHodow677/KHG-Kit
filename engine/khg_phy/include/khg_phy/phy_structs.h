#pragma once

#include "khg_phy/bb.h"
#include "khg_phy/body.h"
#include "khg_phy/constraint.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/spatial_index.h"

struct cpArray {
	int num, max;
	void **arr;
};

struct cpBody {
	// Integration functions
	cpBodyVelocityFunc velocity_func;
	cpBodyPositionFunc position_func;
	
	// mass and it's inverse
	float m;
	float m_inv;
	
	// moment of inertia and it's inverse
	float i;
	float i_inv;
	
	// center of gravity
	cpVect cog;
	
	// position, velocity, force
	cpVect p;
	cpVect v;
	cpVect f;
	
	// Angle, angular velocity, torque (radians)
	float a;
	float w;
	float t;
	
	cpTransform transform;
	
	cpDataPointer userData;
	
	// "pseudo-velocities" used for eliminating overlap.
	// Erin Catto has some papers that talk about what these are.
	cpVect v_bias;
	float w_bias;
	
	cpSpace *space;
	
	cpShape *shapeList;
	cpArbiter *arbiterList;
	cpConstraint *constraintList;
	
	struct {
		cpBody *root;
		cpBody *next;
		float idleTime;
	} sleeping;
};

enum cpArbiterState {
	// Arbiter is active and its the first collision.
	CP_ARBITER_STATE_FIRST_COLLISION,
	// Arbiter is active and its not the first collision.
	CP_ARBITER_STATE_NORMAL,
	// Collision has been explicitly ignored.
	// Either by returning false from a begin collision handler or calling cpArbiterIgnore().
	CP_ARBITER_STATE_IGNORE,
	// Collison is no longer active. A space will cache an arbiter for up to cpSpace.collisionPersistence more steps.
	CP_ARBITER_STATE_CACHED,
	// Collison arbiter is invalid because one of the shapes was removed.
	CP_ARBITER_STATE_INVALIDATED,
};

struct cpArbiterThread {
	struct cpArbiter *next, *prev;
};

struct cpContact {
	cpVect r1, r2;
	
	float nMass, tMass;
	float bounce; // TODO: look for an alternate bounce solution.

	float jnAcc, jtAcc, jBias;
	float bias;
	
	phy_hash_value hash;
};

struct cpCollisionInfo {
	const cpShape *a, *b;
	phy_collision_id id;
	
	cpVect n;
	
	int count;
	// TODO Should this be a unique struct type?
	struct cpContact *arr;
};

struct cpArbiter {
	float e;
	float u;
	cpVect surface_vr;
	
	cpDataPointer data;
	
	const cpShape *a, *b;
	cpBody *body_a, *body_b;
	struct cpArbiterThread thread_a, thread_b;
	
	int count;
	struct cpContact *contacts;
	cpVect n;
	
	// Regular, wildcard A and wildcard B collision handlers.
	cpCollisionHandler *handler, *handlerA, *handlerB;
	bool swapped;
	
	cpTimestamp stamp;
	enum cpArbiterState state;
};

struct cpShapeMassInfo {
	float m;
	float i;
	cpVect cog;
	float area;
};

typedef enum cpShapeType{
	CP_CIRCLE_SHAPE,
	CP_SEGMENT_SHAPE,
	CP_POLY_SHAPE,
	CP_NUM_SHAPES
} cpShapeType;

typedef cpBB (*cpShapeCacheDataImpl)(cpShape *shape, cpTransform transform);
typedef void (*cpShapeDestroyImpl)(cpShape *shape);
typedef void (*cpShapePointQueryImpl)(const cpShape *shape, cpVect p, cpPointQueryInfo *info);
typedef void (*cpShapeSegmentQueryImpl)(const cpShape *shape, cpVect a, cpVect b, float radius, cpSegmentQueryInfo *info);

typedef struct cpShapeClass cpShapeClass;

struct cpShapeClass {
	cpShapeType type;
	
	cpShapeCacheDataImpl cacheData;
	cpShapeDestroyImpl destroy;
	cpShapePointQueryImpl pointQuery;
	cpShapeSegmentQueryImpl segmentQuery;
};

struct cpShape {
	const cpShapeClass *klass;
	
	cpSpace *space;
	cpBody *body;
	struct cpShapeMassInfo massInfo;
	cpBB bb;
	
	bool sensor;
	
	float e;
	float u;
	cpVect surfaceV;

	cpDataPointer userData;
	
	cpCollisionType type;
	cpShapeFilter filter;
	
	cpShape *next;
	cpShape *prev;
	
	phy_hash_value hashid;
};

struct cpCircleShape {
	cpShape shape;
	
	cpVect c, tc;
	float r;
};

struct cpSegmentShape {
	cpShape shape;
	
	cpVect a, b, n;
	cpVect ta, tb, tn;
	float r;
	
	cpVect a_tangent, b_tangent;
};

struct cpSplittingPlane {
	cpVect v0, n;
};

#define CP_POLY_SHAPE_INLINE_ALLOC 6

struct cpPolyShape {
	cpShape shape;
	
	float r;
	
	int count;
	// The untransformed planes are appended at the end of the transformed planes.
	struct cpSplittingPlane *planes;
	
	// Allocate a small number of splitting planes internally for simple poly.
	struct cpSplittingPlane _planes[2*CP_POLY_SHAPE_INLINE_ALLOC];
};

typedef void (*cpConstraintPreStepImpl)(cpConstraint *constraint, float dt);
typedef void (*cpConstraintApplyCachedImpulseImpl)(cpConstraint *constraint, float dt_coef);
typedef void (*cpConstraintApplyImpulseImpl)(cpConstraint *constraint, float dt);
typedef float (*cpConstraintGetImpulseImpl)(cpConstraint *constraint);

typedef struct cpConstraintClass {
	cpConstraintPreStepImpl preStep;
	cpConstraintApplyCachedImpulseImpl applyCachedImpulse;
	cpConstraintApplyImpulseImpl applyImpulse;
	cpConstraintGetImpulseImpl getImpulse;
} cpConstraintClass;

struct cpConstraint {
	const cpConstraintClass *klass;
	
	cpSpace *space;
	
	cpBody *a, *b;
	cpConstraint *next_a, *next_b;
	
	float maxForce;
	float errorBias;
	float maxBias;
	
	bool collideBodies;
	
	cpConstraintPreSolveFunc preSolve;
	cpConstraintPostSolveFunc postSolve;
	
	cpDataPointer userData;
};

struct cpPinJoint {
	cpConstraint constraint;
	cpVect anchorA, anchorB;
	float dist;
	
	cpVect r1, r2;
	cpVect n;
	float nMass;
	
	float jnAcc;
	float bias;
};

struct cpSlideJoint {
	cpConstraint constraint;
	cpVect anchorA, anchorB;
	float min, max;
	
	cpVect r1, r2;
	cpVect n;
	float nMass;
	
	float jnAcc;
	float bias;
};

struct cpPivotJoint {
	cpConstraint constraint;
	cpVect anchorA, anchorB;
	
	cpVect r1, r2;
	cpMat2x2 k;
	
	cpVect jAcc;
	cpVect bias;
};

struct cpGrooveJoint {
	cpConstraint constraint;
	cpVect grv_n, grv_a, grv_b;
	cpVect  anchorB;
	
	cpVect grv_tn;
	float clamp;
	cpVect r1, r2;
	cpMat2x2 k;
	
	cpVect jAcc;
	cpVect bias;
};

struct cpDampedSpring {
	cpConstraint constraint;
	cpVect anchorA, anchorB;
	float restLength;
	float stiffness;
	float damping;
	cpDampedSpringForceFunc springForceFunc;
	
	float target_vrn;
	float v_coef;
	
	cpVect r1, r2;
	float nMass;
	cpVect n;
	
	float jAcc;
};

struct cpDampedRotarySpring {
	cpConstraint constraint;
	float restAngle;
	float stiffness;
	float damping;
	cpDampedRotarySpringTorqueFunc springTorqueFunc;
	
	float target_wrn;
	float w_coef;
	
	float iSum;
	float jAcc;
};

struct cpRotaryLimitJoint {
	cpConstraint constraint;
	float min, max;
	
	float iSum;
		
	float bias;
	float jAcc;
};

struct cpRatchetJoint {
	cpConstraint constraint;
	float angle, phase, ratchet;
	
	float iSum;
		
	float bias;
	float jAcc;
};

struct cpGearJoint {
	cpConstraint constraint;
	float phase, ratio;
	float ratio_inv;
	
	float iSum;
		
	float bias;
	float jAcc;
};

struct cpSimpleMotor {
	cpConstraint constraint;
	float rate;
	
	float iSum;
		
	float jAcc;
};

typedef struct cpContactBufferHeader cpContactBufferHeader;
typedef void (*cpSpaceArbiterApplyImpulseFunc)(cpArbiter *arb);

struct cpSpace {
	int iterations;
	
	cpVect gravity;
	float damping;
	
	float idleSpeedThreshold;
	float sleepTimeThreshold;
	
	float collisionSlop;
	float collisionBias;
	cpTimestamp collisionPersistence;
	
	cpDataPointer userData;
	
	cpTimestamp stamp;
	float curr_dt;

	cpArray *dynamicBodies;
	cpArray *staticBodies;
	cpArray *rousedBodies;
	cpArray *sleepingComponents;
	
	phy_hash_value shapeIDCounter;
	cpSpatialIndex *staticShapes;
	cpSpatialIndex *dynamicShapes;
	
	cpArray *constraints;
	
	cpArray *arbiters;
	cpContactBufferHeader *contactBuffersHead;
	cpHashSet *cachedArbiters;
	cpArray *pooledArbiters;
	
	cpArray *allocatedBuffers;
	int locked;
	
	bool usesWildcards;
	cpHashSet *collisionHandlers;
	cpCollisionHandler defaultHandler;
	
	bool skipPostStep;
	cpArray *postStepCallbacks;
	
	cpBody *staticBody;
	cpBody _staticBody;
};

typedef struct cpPostStepCallback {
	cpPostStepFunc func;
	void *key;
	void *data;
} cpPostStepCallback;
