/* Copyright (c) 2013 Scott Lembcke and Howling Moon Software
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if defined(ANDROID)
#	include <android/log.h>
#endif

#include "khg_phy/phy_private.h"

void
cpMessage(const char *condition, const char *file, int line, int isError, int isHardError, const char *message, ...)
{
	fprintf(stderr, (isError ? "Aborting due to Chipmunk error: " : "Chipmunk warning: "));
	
	va_list vargs;
	va_start(vargs, message); {
#if defined(ANDROID)
		__android_log_print( ANDROID_LOG_INFO, "Chipmunk", "%s(%d)", file, line );
		__android_log_print( ANDROID_LOG_INFO, "Chipmunk", message, vargs );
#else
		vfprintf(stderr, message, vargs);
		fprintf(stderr, "\n");
#endif
	} va_end(vargs);
	
#if defined(ANDROID)
	__android_log_print(ANDROID_LOG_INFO, "Chipmunk", "\tFailed condition: %s\n", condition);
	__android_log_print(ANDROID_LOG_INFO, "Chipmunk", "\tSource:%s:%d\n", file, line);
#else
	fprintf(stderr, "\tFailed condition: %s\n", condition);
	fprintf(stderr, "\tSource:%s:%d\n", file, line);
#endif
}

#define STR(s) #s
#define XSTR(s) STR(s)

const char *cpVersionString = XSTR(CP_VERSION_MAJOR) "." XSTR(CP_VERSION_MINOR) "." XSTR(CP_VERSION_RELEASE);

//MARK: Misc Functions

float
phy_moment_for_circle(float m, float r1, float r2, phy_vect offset)
{
	return m*(0.5f*(r1*r1 + r2*r2) + cpvlengthsq(offset));
}

float
phy_area_for_circle(float r1, float r2)
{
	return (float)CP_PI*phy_abs(r1*r1 - r2*r2);
}

float
phy_moment_for_segment(float m, phy_vect a, phy_vect b, float r)
{
	phy_vect offset = cpvlerp(a, b, 0.5f);
	
	// This approximates the shape as a box for rounded segments, but it's quite close.
	float length = cpvdist(b, a) + 2.0f*r;
	return m*((length*length + 4.0f*r*r)/12.0f + cpvlengthsq(offset));
}

float
phy_area_for_segment(phy_vect a, phy_vect b, float r)
{
	return r*((float)CP_PI*r + 2.0f*cpvdist(a, b));
}

float
phy_moment_for_poly(float m, int count, const phy_vect *verts, phy_vect offset, float r)
{
	// TODO account for radius.
	if(count == 2) return phy_moment_for_segment(m, verts[0], verts[1], 0.0f);
	
	float sum1 = 0.0f;
	float sum2 = 0.0f;
	for(int i=0; i<count; i++){
		phy_vect v1 = cpvadd(verts[i], offset);
		phy_vect v2 = cpvadd(verts[(i+1)%count], offset);
		
		float a = cpvcross(v2, v1);
		float b = cpvdot(v1, v1) + cpvdot(v1, v2) + cpvdot(v2, v2);
		
		sum1 += a*b;
		sum2 += a;
	}
	
	return (m*sum1)/(6.0f*sum2);
}

float
phy_area_for_poly(const int count, const phy_vect *verts, float r)
{
	float area = 0.0f;
	float perimeter = 0.0f;
	for(int i=0; i<count; i++){
		phy_vect v1 = verts[i];
		phy_vect v2 = verts[(i+1)%count];
		
		area += cpvcross(v1, v2);
		perimeter += cpvdist(v1, v2);
	}
	
	return r*(CP_PI*phy_abs(r) + perimeter) + area/2.0f;
}

phy_vect
phy_centroid_for_poly(const int count, const phy_vect *verts)
{
	float sum = 0.0f;
	phy_vect vsum = cpvzero;
	
	for(int i=0; i<count; i++){
		phy_vect v1 = verts[i];
		phy_vect v2 = verts[(i+1)%count];
		float cross = cpvcross(v1, v2);
		
		sum += cross;
		vsum = cpvadd(vsum, cpvmult(cpvadd(v1, v2), cross));
	}
	
	return cpvmult(vsum, 1.0f/(3.0f*sum));
}

//void
//cpRecenterPoly(const int count, phy_vect *verts){
//	phy_vect centroid = cpCentroidForPoly(count, verts);
//	
//	for(int i=0; i<count; i++){
//		verts[i] = cpvsub(verts[i], centroid);
//	}
//}

float
phy_moment_for_box(float m, float width, float height)
{
	return m*(width*width + height*height)/12.0f;
}

float
phy_moment_for_box_2(float m, phy_bb box)
{
	float width = box.r - box.l;
	float height = box.t - box.b;
	phy_vect offset = cpvmult(cpv(box.l + box.r, box.b + box.t), 0.5f);
	
	// TODO: NaN when offset is 0 and m is INFINITY
	return phy_moment_for_box(m, width, height) + m*cpvlengthsq(offset);
}

//MARK: Quick Hull

void
cpLoopIndexes(const phy_vect *verts, int count, int *start, int *end)
{
	(*start) = (*end) = 0;
	phy_vect min = verts[0];
	phy_vect max = min;
	
  for(int i=1; i<count; i++){
    phy_vect v = verts[i];
		
    if(v.x < min.x || (v.x == min.x && v.y < min.y)){
      min = v;
      (*start) = i;
    } else if(v.x > max.x || (v.x == max.x && v.y > max.y)){
			max = v;
			(*end) = i;
		}
	}
}

#define SWAP(__A__, __B__) {phy_vect __TMP__ = __A__; __A__ = __B__; __B__ = __TMP__;}

static int
QHullPartition(phy_vect *verts, int count, phy_vect a, phy_vect b, float tol)
{
	if(count == 0) return 0;
	
	float max = 0;
	int pivot = 0;
	
	phy_vect delta = cpvsub(b, a);
	float valueTol = tol*cpvlength(delta);
	
	int head = 0;
	for(int tail = count-1; head <= tail;){
		float value = cpvcross(cpvsub(verts[head], a), delta);
		if(value > valueTol){
			if(value > max){
				max = value;
				pivot = head;
			}
			
			head++;
		} else {
			SWAP(verts[head], verts[tail]);
			tail--;
		}
	}
	
	// move the new pivot to the front if it's not already there.
	if(pivot != 0) SWAP(verts[0], verts[pivot]);
	return head;
}

static int
QHullReduce(float tol, phy_vect *verts, int count, phy_vect a, phy_vect pivot, phy_vect b, phy_vect *result)
{
	if(count < 0){
		return 0;
	} else if(count == 0) {
		result[0] = pivot;
		return 1;
	} else {
		int left_count = QHullPartition(verts, count, a, pivot, tol);
		int index = QHullReduce(tol, verts + 1, left_count - 1, a, verts[0], pivot, result);
		
		result[index++] = pivot;
		
		int right_count = QHullPartition(verts + left_count, count - left_count, pivot, b, tol);
		return index + QHullReduce(tol, verts + left_count + 1, right_count - 1, pivot, verts[left_count], b, result + index);
	}
}

// QuickHull seemed like a neat algorithm, and efficient-ish for large input sets.
// My implementation performs an in place reduction using the result array as scratch space.
int
phy_convex_hull(int count, const phy_vect *verts, phy_vect *result, int *first, float tol)
{
	if(verts != result){
		// Copy the line vertexes into the empty part of the result polyline to use as a scratch buffer.
		memcpy(result, verts, count*sizeof(phy_vect));
	}
	
	// Degenerate case, all points are the same.
	int start, end;
	cpLoopIndexes(verts, count, &start, &end);
	if(start == end){
		if(first) (*first) = 0;
		return 1;
	}
	
	SWAP(result[0], result[start]);
	SWAP(result[1], result[end == 0 ? start : end]);
	
	phy_vect a = result[0];
	phy_vect b = result[1];
	
	if(first) (*first) = start;
	return QHullReduce(tol, result + 2, count - 2, a, b, a, result + 1) + 1;
}

//MARK: Alternate Block Iterators

#if defined(__has_extension)
#if __has_extension(blocks)

static void IteratorFunc(void *ptr, void (^block)(void *ptr)){block(ptr);}

void cpSpaceEachBody_b(cpSpace *space, void (^block)(cpBody *body)){
	cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)IteratorFunc, block);
}

void cpSpaceEachShape_b(cpSpace *space, void (^block)(cpShape *shape)){
	cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)IteratorFunc, block);
}

void cpSpaceEachConstraint_b(cpSpace *space, void (^block)(cpConstraint *constraint)){
	cpSpaceEachConstraint(space, (cpSpaceConstraintIteratorFunc)IteratorFunc, block);
}

static void BodyIteratorFunc(cpBody *body, void *ptr, void (^block)(void *ptr)){block(ptr);}

void cpBodyEachShape_b(cpBody *body, void (^block)(cpShape *shape)){
	cpBodyEachShape(body, (cpBodyShapeIteratorFunc)BodyIteratorFunc, block);
}

void cpBodyEachConstraint_b(cpBody *body, void (^block)(cpConstraint *constraint)){
	cpBodyEachConstraint(body, (cpBodyConstraintIteratorFunc)BodyIteratorFunc, block);
}

void cpBodyEachArbiter_b(cpBody *body, void (^block)(cpArbiter *arbiter)){
	cpBodyEachArbiter(body, (cpBodyArbiterIteratorFunc)BodyIteratorFunc, block);
}

static void PointQueryIteratorFunc(cpShape *shape, phy_vect p, float d, phy_vect g, cpSpacePointQueryBlock block){block(shape, p, d, g);}
void cpSpacePointQuery_b(cpSpace *space, phy_vect point, float maxDistance, cpShapeFilter filter, cpSpacePointQueryBlock block){
	cpSpacePointQuery(space, point, maxDistance, filter, (cpSpacePointQueryFunc)PointQueryIteratorFunc, block);
}

static void SegmentQueryIteratorFunc(cpShape *shape, phy_vect p, phy_vect n, float t, cpSpaceSegmentQueryBlock block){block(shape, p, n, t);}
void cpSpaceSegmentQuery_b(cpSpace *space, phy_vect start, phy_vect end, float radius, cpShapeFilter filter, cpSpaceSegmentQueryBlock block){
	cpSpaceSegmentQuery(space, start, end, radius, filter, (cpSpaceSegmentQueryFunc)SegmentQueryIteratorFunc, block);
}

void cpSpaceBBQuery_b(cpSpace *space, cpBB bb, cpShapeFilter filter, cpSpaceBBQueryBlock block){
	cpSpaceBBQuery(space, bb, filter, (cpSpaceBBQueryFunc)IteratorFunc, block);
}

static void ShapeQueryIteratorFunc(cpShape *shape, cpContactPointSet *points, cpSpaceShapeQueryBlock block){block(shape, points);}
bool cpSpaceShapeQuery_b(cpSpace *space, cpShape *shape, cpSpaceShapeQueryBlock block){
	return cpSpaceShapeQuery(space, shape, (cpSpaceShapeQueryFunc)ShapeQueryIteratorFunc, block);
}

#endif
#endif
