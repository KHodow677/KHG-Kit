#pragma once

#include "khg_phy/bb.h"
#include "khg_phy/body.h"
#include "khg_phy/constraint.h"
#include "khg_phy/damped_spring.h"
#include "khg_phy/damped_rotary_spring.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/spatial_index.h"

struct phy_array {
	int num, max;
	void **arr;
};

struct phy_body {
	// Integration functions
	phy_body_velocity_func velocity_func;
	phy_body_position_func position_func;
	
	// mass and it's inverse
	float m;
	float m_inv;
	
	// moment of inertia and it's inverse
	float i;
	float i_inv;
	
	// center of gravity
	phy_vect cog;
	
	// position, velocity, force
	phy_vect p;
	phy_vect v;
	phy_vect f;
	
	// Angle, angular velocity, torque (radians)
	float a;
	float w;
	float t;
	
	phy_transform transform;
	
	phy_data_pointer userData;
	
	// "pseudo-velocities" used for eliminating overlap.
	// Erin Catto has some papers that talk about what these are.
	phy_vect v_bias;
	float w_bias;
	
	phy_space *space;
	
	phy_shape *shapeList;
	phy_arbiter *arbiterList;
	phy_constraint *constraintList;
	
	struct {
		phy_body *root;
		phy_body *next;
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
	struct phy_arbiter *next, *prev;
};

struct cpContact {
	phy_vect r1, r2;
	
	float nMass, tMass;
	float bounce; // TODO: look for an alternate bounce solution.

	float jnAcc, jtAcc, jBias;
	float bias;
	
	phy_hash_value hash;
};

struct cpCollisionInfo {
	const phy_shape *a, *b;
	phy_collision_id id;
	
	phy_vect n;
	
	int count;
	// TODO Should this be a unique struct type?
	struct cpContact *arr;
};

struct phy_arbiter {
	float e;
	float u;
	phy_vect surface_vr;
	
	phy_data_pointer data;
	
	const phy_shape *a, *b;
	phy_body *body_a, *body_b;
	struct cpArbiterThread thread_a, thread_b;
	
	int count;
	struct cpContact *contacts;
	phy_vect n;
	
	// Regular, wildcard A and wildcard B collision handlers.
	phy_collision_handler *handler, *handlerA, *handlerB;
	bool swapped;
	
	phy_timestamp stamp;
	enum cpArbiterState state;
};

struct cpShapeMassInfo {
	float m;
	float i;
	phy_vect cog;
	float area;
};

typedef enum cpShapeType{
	CP_CIRCLE_SHAPE,
	CP_SEGMENT_SHAPE,
	CP_POLY_SHAPE,
	CP_NUM_SHAPES
} cpShapeType;

typedef phy_bb (*cpShapeCacheDataImpl)(phy_shape *shape, phy_transform transform);
typedef void (*cpShapeDestroyImpl)(phy_shape *shape);
typedef void (*cpShapePointQueryImpl)(const phy_shape *shape, phy_vect p, cpPointQueryInfo *info);
typedef void (*cpShapeSegmentQueryImpl)(const phy_shape *shape, phy_vect a, phy_vect b, float radius, cpSegmentQueryInfo *info);

typedef struct cpShapeClass cpShapeClass;

struct cpShapeClass {
	cpShapeType type;
	
	cpShapeCacheDataImpl cacheData;
	cpShapeDestroyImpl destroy;
	cpShapePointQueryImpl pointQuery;
	cpShapeSegmentQueryImpl segmentQuery;
};

struct phy_shape {
	const cpShapeClass *klass;
	
	phy_space *space;
	phy_body *body;
	struct cpShapeMassInfo massInfo;
	phy_bb bb;
	
	bool sensor;
	
	float e;
	float u;
	phy_vect surfaceV;

	phy_data_pointer userData;
	
	phy_collision_type type;
	cpShapeFilter filter;
	
	phy_shape *next;
	phy_shape *prev;
	
	phy_hash_value hashid;
};

struct phy_circle_shape {
	phy_shape shape;
	
	phy_vect c, tc;
	float r;
};

struct phy_segment_shape {
	phy_shape shape;
	
	phy_vect a, b, n;
	phy_vect ta, tb, tn;
	float r;
	
	phy_vect a_tangent, b_tangent;
};

struct cpSplittingPlane {
	phy_vect v0, n;
};

#define CP_POLY_SHAPE_INLINE_ALLOC 6

struct phy_poly_shape {
	phy_shape shape;
	
	float r;
	
	int count;
	// The untransformed planes are appended at the end of the transformed planes.
	struct cpSplittingPlane *planes;
	
	// Allocate a small number of splitting planes internally for simple poly.
	struct cpSplittingPlane _planes[2*CP_POLY_SHAPE_INLINE_ALLOC];
};

typedef void (*cpConstraintPreStepImpl)(phy_constraint *constraint, float dt);
typedef void (*cpConstraintApplyCachedImpulseImpl)(phy_constraint *constraint, float dt_coef);
typedef void (*cpConstraintApplyImpulseImpl)(phy_constraint *constraint, float dt);
typedef float (*cpConstraintGetImpulseImpl)(phy_constraint *constraint);

typedef struct cpConstraintClass {
	cpConstraintPreStepImpl preStep;
	cpConstraintApplyCachedImpulseImpl applyCachedImpulse;
	cpConstraintApplyImpulseImpl applyImpulse;
	cpConstraintGetImpulseImpl getImpulse;
} cpConstraintClass;

struct phy_constraint {
	const cpConstraintClass *klass;
	
	phy_space *space;
	
	phy_body *a, *b;
	phy_constraint *next_a, *next_b;
	
	float maxForce;
	float errorBias;
	float maxBias;
	
	bool collideBodies;
	
	phy_constraint_pre_solve_func preSolve;
	phy_constraint_post_solve_func postSolve;
	
	phy_data_pointer userData;
};

struct phy_pin_joint {
	phy_constraint constraint;
	phy_vect anchorA, anchorB;
	float dist;
	
	phy_vect r1, r2;
	phy_vect n;
	float nMass;
	
	float jnAcc;
	float bias;
};

struct phy_slide_joint {
	phy_constraint constraint;
	phy_vect anchorA, anchorB;
	float min, max;
	
	phy_vect r1, r2;
	phy_vect n;
	float nMass;
	
	float jnAcc;
	float bias;
};

struct phy_pivot_joint {
	phy_constraint constraint;
	phy_vect anchorA, anchorB;
	
	phy_vect r1, r2;
	phy_mat2x2 k;
	
	phy_vect jAcc;
	phy_vect bias;
};

struct phy_groove_joint {
	phy_constraint constraint;
	phy_vect grv_n, grv_a, grv_b;
	phy_vect  anchorB;
	
	phy_vect grv_tn;
	float clamp;
	phy_vect r1, r2;
	phy_mat2x2 k;
	
	phy_vect jAcc;
	phy_vect bias;
};

struct phy_damped_spring {
	phy_constraint constraint;
	phy_vect anchorA, anchorB;
	float restLength;
	float stiffness;
	float damping;
	phy_damped_spring_force_func springForceFunc;
	
	float target_vrn;
	float v_coef;
	
	phy_vect r1, r2;
	float nMass;
	phy_vect n;
	
	float jAcc;
};

struct phy_damped_rotary_spring {
	phy_constraint constraint;
	float restAngle;
	float stiffness;
	float damping;
	phy_damped_rotary_spring_torque_func springTorqueFunc;
	
	float target_wrn;
	float w_coef;
	
	float iSum;
	float jAcc;
};

struct rotary_limit_joint {
	phy_constraint constraint;
	float min, max;
	
	float iSum;
		
	float bias;
	float jAcc;
};

struct phy_ratchet_joint {
	phy_constraint constraint;
	float angle, phase, ratchet;
	
	float iSum;
		
	float bias;
	float jAcc;
};

struct phy_gear_joint {
	phy_constraint constraint;
	float phase, ratio;
	float ratio_inv;
	
	float iSum;
		
	float bias;
	float jAcc;
};

struct phy_simple_motor_joint {
	phy_constraint constraint;
	float rate;
	
	float iSum;
		
	float jAcc;
};

typedef struct cpContactBufferHeader cpContactBufferHeader;
typedef void (*cpSpaceArbiterApplyImpulseFunc)(phy_arbiter *arb);

struct phy_space {
	int iterations;
	
	phy_vect gravity;
	float damping;
	
	float idleSpeedThreshold;
	float sleepTimeThreshold;
	
	float collisionSlop;
	float collisionBias;
	phy_timestamp collisionPersistence;
	
	phy_data_pointer userData;
	
	phy_timestamp stamp;
	float curr_dt;

	phy_array *dynamicBodies;
	phy_array *staticBodies;
	phy_array *rousedBodies;
	phy_array *sleepingComponents;
	
	phy_hash_value shapeIDCounter;
	cpSpatialIndex *staticShapes;
	cpSpatialIndex *dynamicShapes;
	
	phy_array *constraints;
	
	phy_array *arbiters;
	cpContactBufferHeader *contactBuffersHead;
	phy_hash_set *cachedArbiters;
	phy_array *pooledArbiters;
	
	phy_array *allocatedBuffers;
	int locked;
	
	bool usesWildcards;
	phy_hash_set *collisionHandlers;
	phy_collision_handler defaultHandler;
	
	bool skipPostStep;
	phy_array *postStepCallbacks;
	
	phy_body *staticBody;
	phy_body _staticBody;
};

typedef struct cpPostStepCallback {
	cpPostStepFunc func;
	void *key;
	void *data;
} cpPostStepCallback;
