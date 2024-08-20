#pragma once

#include "khg_phy/phy.h"
#include "khg_phy/phy_structs.h"

#define CP_HASH_COEF (3344921057ul)
#define CP_HASH_PAIR(A, B) ((phy_hash_value)(A)*CP_HASH_COEF ^ (phy_hash_value)(B)*CP_HASH_COEF)

// TODO: Eww. Magic numbers.
#define MAGIC_EPSILON 1e-5


//MARK: cpArray

phy_array *cpArrayNew(int size);

void cpArrayFree(phy_array *arr);

void cpArrayPush(phy_array *arr, void *object);
void *cpArrayPop(phy_array *arr);
void cpArrayDeleteObj(phy_array *arr, void *obj);
bool cpArrayContains(phy_array *arr, void *ptr);

void cpArrayFreeEach(phy_array *arr, void (freeFunc)(void*));


//MARK: cpHashSet

typedef bool (*cpHashSetEqlFunc)(const void *ptr, const void *elt);
typedef void *(*cpHashSetTransFunc)(const void *ptr, void *data);

phy_hash_set *cpHashSetNew(int size, cpHashSetEqlFunc eqlFunc);
void cpHashSetSetDefaultValue(phy_hash_set *set, void *default_value);

void cpHashSetFree(phy_hash_set *set);

int cpHashSetCount(phy_hash_set *set);
const void *cpHashSetInsert(phy_hash_set *set, phy_hash_value hash, const void *ptr, cpHashSetTransFunc trans, void *data);
const void *cpHashSetRemove(phy_hash_set *set, phy_hash_value hash, const void *ptr);
const void *cpHashSetFind(phy_hash_set *set, phy_hash_value hash, const void *ptr);

typedef void (*cpHashSetIteratorFunc)(void *elt, void *data);
void cpHashSetEach(phy_hash_set *set, cpHashSetIteratorFunc func, void *data);

typedef bool (*cpHashSetFilterFunc)(void *elt, void *data);
void cpHashSetFilter(phy_hash_set *set, cpHashSetFilterFunc func, void *data);


//MARK: Bodies

void cpBodyAddShape(phy_body *body, phy_shape *shape);
void cpBodyRemoveShape(phy_body *body, phy_shape *shape);

//void cpBodyAccumulateMassForShape(cpBody *body, phy_shape *shape);
void cpBodyAccumulateMassFromShapes(phy_body *body);

void cpBodyRemoveConstraint(phy_body *body, phy_constraint *constraint);


//MARK: Spatial Index Functions

cpSpatialIndex *cpSpatialIndexInit(cpSpatialIndex *index, cpSpatialIndexClass *klass, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex);


//MARK: Arbiters

phy_arbiter* cpArbiterInit(phy_arbiter *arb, phy_shape *a, phy_shape *b);

static inline struct cpArbiterThread *
cpArbiterThreadForBody(phy_arbiter *arb, phy_body *body)
{
	return (arb->body_a == body ? &arb->thread_a : &arb->thread_b);
}

void cpArbiterUnthread(phy_arbiter *arb);

void cpArbiterUpdate(phy_arbiter *arb, struct cpCollisionInfo *info, phy_space *space);
void cpArbiterPreStep(phy_arbiter *arb, float dt, float bias, float slop);
void cpArbiterApplyCachedImpulse(phy_arbiter *arb, float dt_coef);
void cpArbiterApplyImpulse(phy_arbiter *arb);


//MARK: Shapes/Collisions

phy_shape *cpShapeInit(phy_shape *shape, const cpShapeClass *klass, phy_body *body, struct cpShapeMassInfo massInfo);

static inline bool
cpShapeActive(phy_shape *shape)
{
	// checks if the shape is added to a shape list.
	// TODO could this just check the space now?
	return (shape->prev || (shape->body && shape->body->shapeList == shape));
}

// Note: This function returns contact points with r1/r2 in absolute coordinates, not body relative.
struct cpCollisionInfo cpCollide(const phy_shape *a, const phy_shape *b, phy_collision_id id, struct cpContact *contacts);

static inline void
CircleSegmentQuery(phy_shape *shape, phy_vect center, float r1, phy_vect a, phy_vect b, float r2, cpSegmentQueryInfo *info)
{
	phy_vect da = cpvsub(a, center);
	phy_vect db = cpvsub(b, center);
	float rsum = r1 + r2;
	
	float qa = cpvdot(da, da) - 2.0f*cpvdot(da, db) + cpvdot(db, db);
	float qb = cpvdot(da, db) - cpvdot(da, da);
	float det = qb*qb - qa*(cpvdot(da, da) - rsum*rsum);
	
	if(det >= 0.0f){
		float t = (-qb - sqrtf(det))/(qa);
		if(0.0f<= t && t <= 1.0f){
			phy_vect n = cpvnormalize(cpvlerp(da, db, t));
			
			info->shape = shape;
			info->point = cpvsub(cpvlerp(a, b, t), cpvmult(n, r2));
			info->normal = n;
			info->alpha = t;
		}
	}
}

static inline bool
cpShapeFilterReject(cpShapeFilter a, cpShapeFilter b)
{
	// Reject the collision if:
	return (
		// They are in the same non-zero group.
		(a.group != 0 && a.group == b.group) ||
		// One of the category/mask combinations fails.
		(a.categories & b.mask) == 0 ||
		(b.categories & a.mask) == 0
	);
}

void cpLoopIndexes(const phy_vect *verts, int count, int *start, int *end);


//MARK: Constraints
// TODO naming conventions here

void cpConstraintInit(phy_constraint *constraint, const struct cpConstraintClass *klass, phy_body *a, phy_body *b);

static inline void
cpConstraintActivateBodies(phy_constraint *constraint)
{
	phy_body *a = constraint->a; phy_body_activate(a);
	phy_body *b = constraint->b; phy_body_activate(b);
}

static inline phy_vect
relative_velocity(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2){
	phy_vect v1_sum = cpvadd(a->v, cpvmult(cpvperp(r1), a->w));
	phy_vect v2_sum = cpvadd(b->v, cpvmult(cpvperp(r2), b->w));
	
	return cpvsub(v2_sum, v1_sum);
}

static inline float
normal_relative_velocity(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2, phy_vect n){
	return cpvdot(relative_velocity(a, b, r1, r2), n);
}

static inline void
apply_impulse(phy_body *body, phy_vect j, phy_vect r){
	body->v = cpvadd(body->v, cpvmult(j, body->m_inv));
	body->w += body->i_inv*cpvcross(r, j);
}

static inline void
apply_impulses(phy_body *a , phy_body *b, phy_vect r1, phy_vect r2, phy_vect j)
{
	apply_impulse(a, cpvneg(j), r1);
	apply_impulse(b, j, r2);
}

static inline void
apply_bias_impulse(phy_body *body, phy_vect j, phy_vect r)
{
	body->v_bias = cpvadd(body->v_bias, cpvmult(j, body->m_inv));
	body->w_bias += body->i_inv*cpvcross(r, j);
}

static inline void
apply_bias_impulses(phy_body *a , phy_body *b, phy_vect r1, phy_vect r2, phy_vect j)
{
	apply_bias_impulse(a, cpvneg(j), r1);
	apply_bias_impulse(b, j, r2);
}

static inline float
k_scalar_body(phy_body *body, phy_vect r, phy_vect n)
{
	float rcn = cpvcross(r, n);
	return body->m_inv + body->i_inv*rcn*rcn;
}

static inline float
k_scalar(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2, phy_vect n)
{
	float value = k_scalar_body(a, r1, n) + k_scalar_body(b, r2, n);
	cpAssertSoft(value != 0.0, "Unsolvable collision or constraint.");
	
	return value;
}

static inline phy_mat2x2
k_tensor(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2)
{
	float m_sum = a->m_inv + b->m_inv;
	
	// start with Identity*m_sum
	float k11 = m_sum, k12 = 0.0f;
	float k21 = 0.0f,  k22 = m_sum;
	
	// add the influence from r1
	float a_i_inv = a->i_inv;
	float r1xsq =  r1.x * r1.x * a_i_inv;
	float r1ysq =  r1.y * r1.y * a_i_inv;
	float r1nxy = -r1.x * r1.y * a_i_inv;
	k11 += r1ysq; k12 += r1nxy;
	k21 += r1nxy; k22 += r1xsq;
	
	// add the influnce from r2
	float b_i_inv = b->i_inv;
	float r2xsq =  r2.x * r2.x * b_i_inv;
	float r2ysq =  r2.y * r2.y * b_i_inv;
	float r2nxy = -r2.x * r2.y * b_i_inv;
	k11 += r2ysq; k12 += r2nxy;
	k21 += r2nxy; k22 += r2xsq;
	
	// invert
	float det = k11*k22 - k12*k21;
	cpAssertSoft(det != 0.0, "Unsolvable constraint.");
	
	float det_inv = 1.0f/det;
	return cpMat2x2New(
		 k22*det_inv, -k12*det_inv,
		-k21*det_inv,  k11*det_inv
 	);
}

static inline float
bias_coef(float errorBias, float dt)
{
	return 1.0f - powf(errorBias, dt);
}


//MARK: Spaces

#define cpAssertSpaceUnlocked(space) \
	cpAssertHard(!space->locked, \
		"This operation cannot be done safely during a call to cpSpaceStep() or during a query. " \
		"Put these calls into a post-step callback." \
	);

void cpSpaceSetStaticBody(phy_space *space, phy_body *body);

extern phy_collision_handler cpCollisionHandlerDoNothing;

void cpSpaceProcessComponents(phy_space *space, float dt);

void cpSpacePushFreshContactBuffer(phy_space *space);
struct cpContact *cpContactBufferGetArray(phy_space *space);
void cpSpacePushContacts(phy_space *space, int count);

cpPostStepCallback *cpSpaceGetPostStepCallback(phy_space *space, void *key);

bool cpSpaceArbiterSetFilter(phy_arbiter *arb, phy_space *space);
void cpSpaceFilterArbiters(phy_space *space, phy_body *body, phy_shape *filter);

void cpSpaceActivateBody(phy_space *space, phy_body *body);
void cpSpaceLock(phy_space *space);
void cpSpaceUnlock(phy_space *space, bool runPostStep);

static inline void
cpSpaceUncacheArbiter(phy_space *space, phy_arbiter *arb)
{
	const phy_shape *a = arb->a, *b = arb->b;
	const phy_shape *shape_pair[] = {a, b};
	phy_hash_value arbHashID = CP_HASH_PAIR((phy_hash_value)a, (phy_hash_value)b);
	cpHashSetRemove(space->cachedArbiters, arbHashID, shape_pair);
	cpArrayDeleteObj(space->arbiters, arb);
}

static inline phy_array *
cpSpaceArrayForBodyType(phy_space *space, phy_body_type type)
{
	return (type == CP_BODY_TYPE_STATIC ? space->staticBodies : space->dynamicBodies);
}

void cpShapeUpdateFunc(phy_shape *shape, void *unused);
phy_collision_id cpSpaceCollideShapes(phy_shape *a, phy_shape *b, phy_collision_id id, phy_space *space);


//MARK: Foreach loops

static inline phy_constraint *
cpConstraintNext(phy_constraint *node, phy_body *body)
{
	return (node->a == body ? node->next_a : node->next_b);
}

#define CP_BODY_FOREACH_CONSTRAINT(bdy, var)\
	for(phy_constraint *var = bdy->constraintList; var; var = cpConstraintNext(var, bdy))

static inline phy_arbiter *
cpArbiterNext(phy_arbiter *node, phy_body *body)
{
	return (node->body_a == body ? node->thread_a.next : node->thread_b.next);
}

#define CP_BODY_FOREACH_ARBITER(bdy, var)\
	for(phy_arbiter *var = bdy->arbiterList; var; var = cpArbiterNext(var, bdy))

#define CP_BODY_FOREACH_SHAPE(body, var)\
	for(phy_shape *var = body->shapeList; var; var = var->next)

#define CP_BODY_FOREACH_COMPONENT(root, var)\
	for(phy_body *var = root; var; var = var->sleeping.next)
