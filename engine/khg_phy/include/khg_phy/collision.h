#pragma once

#include "khg_phy/base.h"
#include "khg_phy/math_functions.h"
#include <stdbool.h>

typedef struct b2Circle b2Circle;
typedef struct b2Capsule b2Capsule;
typedef struct b2DistanceCache b2DistanceCache;
typedef struct b2Polygon b2Polygon;
typedef struct b2Segment b2Segment;
typedef struct b2SmoothSegment b2SmoothSegment;
typedef struct b2Hull b2Hull;

#define b2_maxPolygonVertices 8

typedef struct b2RayCastInput {
	b2Vec2 origin;
	b2Vec2 translation;
	float maxFraction;
} b2RayCastInput;

typedef struct b2ShapeCastInput {
	b2Vec2 points[b2_maxPolygonVertices];
	int32_t count;
	float radius;
	b2Vec2 translation;
	float maxFraction;
} b2ShapeCastInput;

typedef struct b2CastOutput {
	b2Vec2 normal;
	b2Vec2 point;
	float fraction;
	int32_t iterations;
	bool hit;
} b2CastOutput;

typedef struct b2MassData {
	float mass;
	b2Vec2 center;
	float I;
} b2MassData;

typedef struct b2Circle {
	b2Vec2 center;
	float radius;
} b2Circle;

typedef struct b2Capsule {
	b2Vec2 center1;
	b2Vec2 center2;
	float radius;
} b2Capsule;

typedef struct b2Polygon {
	b2Vec2 vertices[b2_maxPolygonVertices];
	b2Vec2 normals[b2_maxPolygonVertices];
	b2Vec2 centroid;
	float radius;
	int32_t count;
} b2Polygon;

typedef struct b2Segment {
	b2Vec2 point1;
	b2Vec2 point2;
} b2Segment;

typedef struct b2SmoothSegment {
	b2Vec2 ghost1;
	b2Segment segment;
	b2Vec2 ghost2;
	int32_t chainId;
} b2SmoothSegment;

B2_API bool b2IsValidRay(const b2RayCastInput *input);

B2_API b2Polygon b2MakePolygon(const b2Hull *hull, float radius);
B2_API b2Polygon b2MakeOffsetPolygon(const b2Hull *hull, float radius, b2Transform transform);
B2_API b2Polygon b2MakeSquare(float h);
B2_API b2Polygon b2MakeBox(float hx, float hy);
B2_API b2Polygon b2MakeRoundedBox(float hx, float hy, float radius);
B2_API b2Polygon b2MakeOffsetBox(float hx, float hy, b2Vec2 center, float angle);
B2_API b2Polygon b2TransformPolygon(b2Transform transform, const b2Polygon *polygon);

B2_API b2MassData b2ComputeCircleMass(const b2Circle *shape, float density);
B2_API b2MassData b2ComputeCapsuleMass(const b2Capsule *shape, float density);
B2_API b2MassData b2ComputePolygonMass(const b2Polygon *shape, float density);

B2_API b2AABB b2ComputeCircleAABB(const b2Circle *shape, b2Transform transform);
B2_API b2AABB b2ComputeCapsuleAABB(const b2Capsule *shape, b2Transform transform);
B2_API b2AABB b2ComputePolygonAABB(const b2Polygon *shape, b2Transform transform);
B2_API b2AABB b2ComputeSegmentAABB(const b2Segment *shape, b2Transform transform);

B2_API bool b2PointInCircle(b2Vec2 point, const b2Circle *shape);
B2_API bool b2PointInCapsule(b2Vec2 point, const b2Capsule *shape);
B2_API bool b2PointInPolygon(b2Vec2 point, const b2Polygon *shape);
B2_API b2CastOutput b2RayCastCircle(const b2RayCastInput *input, const b2Circle *shape);
B2_API b2CastOutput b2RayCastCapsule(const b2RayCastInput *input, const b2Capsule *shape);
B2_API b2CastOutput b2RayCastSegment(const b2RayCastInput *input, const b2Segment *shape, bool oneSided);
B2_API b2CastOutput b2RayCastPolygon(const b2RayCastInput *input, const b2Polygon *shape);

B2_API b2CastOutput b2ShapeCastCircle(const b2ShapeCastInput *input, const b2Circle *shape);
B2_API b2CastOutput b2ShapeCastCapsule(const b2ShapeCastInput *input, const b2Capsule *shape);
B2_API b2CastOutput b2ShapeCastSegment(const b2ShapeCastInput *input, const b2Segment *shape);
B2_API b2CastOutput b2ShapeCastPolygon(const b2ShapeCastInput *input, const b2Polygon *shape);

typedef struct b2Hull {
	b2Vec2 points[b2_maxPolygonVertices];
	int32_t count;
} b2Hull;

B2_API b2Hull b2ComputeHull(const b2Vec2 *points, int32_t count);
B2_API bool b2ValidateHull(const b2Hull *hull);

typedef struct b2SegmentDistanceResult {
	b2Vec2 closest1;
	b2Vec2 closest2;
	float fraction1;
	float fraction2;
	float distanceSquared;
} b2SegmentDistanceResult;

B2_API b2SegmentDistanceResult b2SegmentDistance(b2Vec2 p1, b2Vec2 q1, b2Vec2 p2, b2Vec2 q2);

typedef struct b2DistanceProxy {
	b2Vec2 points[b2_maxPolygonVertices];
	int32_t count;
	float radius;
} b2DistanceProxy;

typedef struct b2DistanceCache {
	float metric; 
	uint16_t count;
	uint8_t indexA[3];
	uint8_t indexB[3];
} b2DistanceCache;

static const b2DistanceCache b2_emptyDistanceCache = B2_ZERO_INIT;

typedef struct b2DistanceInput {
	b2DistanceProxy proxyA;
	b2DistanceProxy proxyB;
	b2Transform transformA;
	b2Transform transformB;
	bool useRadii;
} b2DistanceInput;

typedef struct b2DistanceOutput {
	b2Vec2 pointA;
	b2Vec2 pointB;
	float distance;
	int32_t iterations;
} b2DistanceOutput;

B2_API b2DistanceOutput b2ShapeDistance(b2DistanceCache *cache, const b2DistanceInput *input);
typedef struct b2ShapeCastPairInput {
	b2DistanceProxy proxyA;
	b2DistanceProxy proxyB;
	b2Transform transformA;
	b2Transform transformB;
	b2Vec2 translationB;
	float maxFraction;
} b2ShapeCastPairInput;

B2_API b2CastOutput b2ShapeCast(const b2ShapeCastPairInput *input);
B2_API b2DistanceProxy b2MakeProxy(const b2Vec2 *vertices, int32_t count, float radius);

typedef struct b2Sweep {
	b2Vec2 localCenter;
	b2Vec2 c1;
	b2Vec2 c2;
	b2Rot q1;
	b2Rot q2;
} b2Sweep;

B2_API b2Transform b2GetSweepTransform(const b2Sweep *sweep, float time);

typedef struct b2TOIInput {
	b2DistanceProxy proxyA;
	b2DistanceProxy proxyB;
	b2Sweep sweepA;
	b2Sweep sweepB;
	float tMax;
} b2TOIInput;

typedef enum b2TOIState {
	b2_toiStateUnknown,
	b2_toiStateFailed,
	b2_toiStateOverlapped,
	b2_toiStateHit,
	b2_toiStateSeparated
} b2TOIState;

typedef struct b2TOIOutput {
	b2TOIState state;
	float t;
} b2TOIOutput;

B2_API b2TOIOutput b2TimeOfImpact(const b2TOIInput *input);

typedef struct b2ManifoldPoint {
	b2Vec2 point;
	b2Vec2 anchorA;
	b2Vec2 anchorB;
	float separation;
	float normalImpulse;
	float tangentImpulse;
	float maxNormalImpulse;
	float normalVelocity;
	uint16_t id;
	bool persisted;
} b2ManifoldPoint;

typedef struct b2Manifold {
	b2ManifoldPoint points[2];
	b2Vec2 normal;
	int32_t pointCount;
} b2Manifold;

B2_API b2Manifold b2CollideCircles(const b2Circle *circleA, b2Transform xfA, const b2Circle *circleB, b2Transform xfB);
B2_API b2Manifold b2CollideCapsuleAndCircle(const b2Capsule *capsuleA, b2Transform xfA, const b2Circle *circleB, b2Transform xfB);
B2_API b2Manifold b2CollideSegmentAndCircle(const b2Segment *segmentA, b2Transform xfA, const b2Circle *circleB, b2Transform xfB);
B2_API b2Manifold b2CollidePolygonAndCircle(const b2Polygon *polygonA, b2Transform xfA, const b2Circle *circleB, b2Transform xfB);
B2_API b2Manifold b2CollideCapsules(const b2Capsule *capsuleA, b2Transform xfA, const b2Capsule *capsuleB, b2Transform xfB, b2DistanceCache *cache);
B2_API b2Manifold b2CollideSegmentAndCapsule(const b2Segment *segmentA, b2Transform xfA, const b2Capsule *capsuleB, b2Transform xfB, b2DistanceCache *cache);
B2_API b2Manifold b2CollidePolygonAndCapsule(const b2Polygon *polygonA, b2Transform xfA, const b2Capsule *capsuleB, b2Transform xfB, b2DistanceCache *cache);
B2_API b2Manifold b2CollidePolygons(const b2Polygon *polyA, b2Transform xfA, const b2Polygon *polyB, b2Transform xfB, b2DistanceCache *cache);

B2_API b2Manifold b2CollideSegmentAndPolygon(const b2Segment *segmentA, b2Transform xfA, const b2Polygon *polygonB, b2Transform xfB, b2DistanceCache *cache);
B2_API b2Manifold b2CollideSmoothSegmentAndCircle(const b2SmoothSegment *smoothSegmentA, b2Transform xfA, const b2Circle *circleB, b2Transform xfB);
B2_API b2Manifold b2CollideSmoothSegmentAndCapsule(const b2SmoothSegment *smoothSegmentA, b2Transform xfA, const b2Capsule *capsuleB, b2Transform xfB, b2DistanceCache *cache);
B2_API b2Manifold b2CollideSmoothSegmentAndPolygon(const b2SmoothSegment *smoothSegmentA, b2Transform xfA, const b2Polygon *polygonB, b2Transform xfB, b2DistanceCache *cache);

#define b2_defaultCategoryBits (0x00000001)
#define b2_defaultMaskBits (0xFFFFFFFF)

typedef struct b2TreeNode {
	b2AABB aabb;
	uint32_t categoryBits;
	union {
		int32_t parent;
		int32_t next;
	};
	int32_t child1;
	int32_t child2;
	int32_t userData;
	int16_t height;
	bool enlarged;
	char pad[9];
} b2TreeNode;

typedef struct b2DynamicTree {
	b2TreeNode *nodes;
	int32_t root;
	int32_t nodeCount;
	int32_t nodeCapacity;
	int32_t freeList;
	int32_t proxyCount;
	int32_t *leafIndices;
	b2AABB *leafBoxes;
	b2Vec2 *leafCenters;
	int32_t *binIndices;
	int32_t rebuildCapacity;
} b2DynamicTree;

B2_API b2DynamicTree b2DynamicTree_Create(void);
B2_API void b2DynamicTree_Destroy(b2DynamicTree *tree);
B2_API int32_t b2DynamicTree_CreateProxy(b2DynamicTree *tree, b2AABB aabb, uint32_t categoryBits, int32_t userData);
B2_API void b2DynamicTree_DestroyProxy(b2DynamicTree *tree, int32_t proxyId);
B2_API void b2DynamicTree_MoveProxy(b2DynamicTree *tree, int32_t proxyId, b2AABB aabb);
B2_API void b2DynamicTree_EnlargeProxy(b2DynamicTree *tree, int32_t proxyId, b2AABB aabb);

typedef bool b2TreeQueryCallbackFcn(int32_t proxyId, int32_t userData, void *context);
B2_API void b2DynamicTree_Query(const b2DynamicTree *tree, b2AABB aabb, uint32_t maskBits, b2TreeQueryCallbackFcn *callback, void *context);
typedef float b2TreeRayCastCallbackFcn(const b2RayCastInput *input, int32_t proxyId, int32_t userData, void *context);
B2_API void b2DynamicTree_RayCast(const b2DynamicTree *tree, const b2RayCastInput *input, uint32_t maskBits, b2TreeRayCastCallbackFcn *callback, void *context);
typedef float b2TreeShapeCastCallbackFcn(const b2ShapeCastInput *input, int32_t proxyId, int32_t userData, void *context);
B2_API void b2DynamicTree_ShapeCast(const b2DynamicTree *tree, const b2ShapeCastInput *input, uint32_t maskBits, b2TreeShapeCastCallbackFcn *callback, void *context);

B2_API void b2DynamicTree_Validate(const b2DynamicTree *tree);
B2_API int b2DynamicTree_GetHeight(const b2DynamicTree *tree);
B2_API int b2DynamicTree_GetMaxBalance(const b2DynamicTree *tree);
B2_API float b2DynamicTree_GetAreaRatio(const b2DynamicTree *tree);
B2_API void b2DynamicTree_RebuildBottomUp(b2DynamicTree *tree);
B2_API int b2DynamicTree_GetProxyCount(const b2DynamicTree *tree);
B2_API int b2DynamicTree_Rebuild(b2DynamicTree *tree, bool fullBuild);
B2_API void b2DynamicTree_ShiftOrigin(b2DynamicTree *tree, b2Vec2 newOrigin);
B2_API int b2DynamicTree_GetByteCount(const b2DynamicTree *tree);

B2_INLINE int32_t b2DynamicTree_GetUserData(const b2DynamicTree *tree, int32_t proxyId) {
	return tree->nodes[proxyId].userData;
}

B2_INLINE b2AABB b2DynamicTree_GetAABB(const b2DynamicTree *tree, int32_t proxyId)
{
	return tree->nodes[proxyId].aabb;
}
