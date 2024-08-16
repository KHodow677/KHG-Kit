// Copyright 2013 Howling Moon Software. All rights reserved.
// See http://chipmunk2d.net/legal.php for more information.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "khg_phy/phy_private.h"
#include "khg_phy/polyline.h"


static inline int Next(int i, int count){return (i+1)%count;}

//MARK: Polylines

#define DEFAULT_POLYLINE_CAPACITY 16

static int
cpPolylineSizeForCapacity(int capacity)
{
	return sizeof(cpPolyline) + capacity*sizeof(phy_vect);
}

static cpPolyline *
cpPolylineMake(int capacity)
{
	capacity = (capacity > DEFAULT_POLYLINE_CAPACITY ? capacity : DEFAULT_POLYLINE_CAPACITY);
	
	cpPolyline *line = (cpPolyline *)calloc(1, cpPolylineSizeForCapacity(capacity));
	line->count = 0;
	line->capacity = capacity;
	
	return line;
}

static cpPolyline *
cpPolylineMake2(int capacity, phy_vect a, phy_vect b)
{
	cpPolyline *line = cpPolylineMake(capacity);
	line->count = 2;
	line->verts[0] = a;
	line->verts[1] = b;
	
	return line;
}

static cpPolyline *
cpPolylineShrink(cpPolyline *line)
{
	line->capacity = line->count;
	return (cpPolyline*)realloc(line, cpPolylineSizeForCapacity(line->count));
}

void
cpPolylineFree(cpPolyline *line)
{
	free(line);
}

// Grow the allocated memory for a polyline.
static cpPolyline *
cpPolylineGrow(cpPolyline *line, int count)
{
  line->count += count;
  
  int capacity = line->capacity;
  while(line->count > capacity) capacity *= 2;
  
  if(line->capacity < capacity){
    line->capacity = capacity;
		line = (cpPolyline*)realloc(line, cpPolylineSizeForCapacity(capacity));
  }
	
	return line;
}

// Push v onto the end of line.
static cpPolyline *
cpPolylinePush(cpPolyline *line, phy_vect v)
{
  int count = line->count;
  line = cpPolylineGrow(line, 1);
  line->verts[count] = v;
	
	return line;
}

// Push v onto the beginning of line.
static cpPolyline *
cpPolylineEnqueue(cpPolyline *line, phy_vect v)
{
	// TODO could optimize this to grow in both directions.
	// Probably doesn't matter though.
  int count = line->count;
  line = cpPolylineGrow(line, 1);
  memmove(line->verts + 1, line->verts, count*sizeof(phy_vect));
  line->verts[0] = v;
	
	return line;
}

// Returns true if the polyline starts and ends with the same vertex.
bool
cpPolylineIsClosed(cpPolyline *line)
{
	return (line->count > 1 && cpveql(line->verts[0], line->verts[line->count-1]));
}

// Check if a cpPolyline is longer than a certain length
// Takes a range which can wrap around if the polyline is looped.
static bool
cpPolylineIsShort(phy_vect *points, int count, int start, int end, float min)
{
  float length = 0.0f;
	for(int i=start; i!=end; i=Next(i, count)){
		length += cpvdist(points[i], points[Next(i, count)]);
		if(length > min) return false;
	}
  
  return true;
}

//MARK: Polyline Simplification

static inline float
Sharpness(phy_vect a, phy_vect b, phy_vect c)
{
	// TODO could speed this up by caching the normals instead of calculating each twice.
  return cpvdot(cpvnormalize(cpvsub(a, b)), cpvnormalize(cpvsub(c, b)));
}

// Join similar adjacent line segments together. Works well for hard edged shapes.
// 'tol' is the minimum anglular difference in radians of a vertex.
cpPolyline *
cpPolylineSimplifyVertexes(cpPolyline *line, float tol)
{
	cpPolyline *reduced = cpPolylineMake2(0, line->verts[0], line->verts[1]);
	
	float minSharp = -cosf(tol);
	
	for(int i=2; i<line->count; i++){
		phy_vect vert = line->verts[i];
		float sharp = Sharpness(reduced->verts[reduced->count - 2], reduced->verts[reduced->count - 1], vert);
		
		if(sharp <= minSharp){
			reduced->verts[reduced->count - 1] = vert;
		} else {
			reduced = cpPolylinePush(reduced, vert);
		}
	}
	
	if(
		cpPolylineIsClosed(line) &&
		Sharpness(reduced->verts[reduced->count - 2], reduced->verts[0], reduced->verts[1]) < minSharp
	){
		reduced->verts[0] = reduced->verts[reduced->count - 2];
		reduced->count--;
	}
	
	// TODO shrink
	return reduced;
}

// Recursive function used by cpPolylineSimplifyCurves().
static cpPolyline *
DouglasPeucker(
	phy_vect *verts, cpPolyline *reduced,
	int length, int start, int end,
	float min, float tol
){
	// Early exit if the points are adjacent
  if((end - start + length)%length < 2) return reduced;
  
	phy_vect a = verts[start];
	phy_vect b = verts[end];
	
	// Check if the length is below the threshold
	if(cpvnear(a, b, min) && cpPolylineIsShort(verts, length, start, end, min)) return reduced;
	
	// Find the maximal vertex to split and recurse on
	float max = 0.0;
	int maxi = start;
	
	phy_vect n = cpvnormalize(cpvperp(cpvsub(b, a)));
	float d = cpvdot(n, a);
	
	for(int i=Next(start, length); i!=end; i=Next(i, length)){
		float dist = fabs(cpvdot(n, verts[i]) - d);
		
		if(dist > max){
			max = dist;
			maxi = i;
		}
	}
	
	if(max > tol){
    reduced = DouglasPeucker(verts, reduced, length, start, maxi, min, tol);
		reduced = cpPolylinePush(reduced, verts[maxi]);
    reduced = DouglasPeucker(verts, reduced, length, maxi, end, min, tol);
	}
	
	return reduced;
}

// Recursively reduce the vertex count on a polyline. Works best for smooth shapes.
// 'tol' is the maximum error for the reduction.
// The reduced polyline will never be farther than this distance from the original polyline.
cpPolyline *
cpPolylineSimplifyCurves(cpPolyline *line, float tol)
{
	cpPolyline *reduced = cpPolylineMake(line->count);
	
	float min = tol/2.0f;
  
  if(cpPolylineIsClosed(line)){
		int start, end;
    cpLoopIndexes(line->verts, line->count - 1, &start, &end);
    
		reduced = cpPolylinePush(reduced, line->verts[start]);
		reduced = DouglasPeucker(line->verts, reduced, line->count - 1, start, end, min, tol);
		reduced = cpPolylinePush(reduced, line->verts[end]);
		reduced = DouglasPeucker(line->verts, reduced, line->count - 1, end, start, min, tol);
		reduced = cpPolylinePush(reduced, line->verts[start]);
  } else {
		reduced = cpPolylinePush(reduced, line->verts[0]);
		reduced = DouglasPeucker(line->verts, reduced, line->count, 0, line->count - 1, min, tol);
		reduced = cpPolylinePush(reduced, line->verts[line->count - 1]);
  }
	
	return cpPolylineShrink(reduced);
}

//MARK: Polyline Sets

cpPolylineSet *
cpPolylineSetAlloc(void)
{
	return (cpPolylineSet *)calloc(1, sizeof(cpPolylineSet));
}

cpPolylineSet *
cpPolylineSetInit(cpPolylineSet *set)
{
	set->count = 0;
	set->capacity = 8;
	set->lines = (cpPolyline**)calloc(set->capacity, sizeof(cpPolyline));
	
  return set;
}


cpPolylineSet *
cpPolylineSetNew(void)
{
	return cpPolylineSetInit(cpPolylineSetAlloc());
}

void
cpPolylineSetDestroy(cpPolylineSet *set, bool freePolylines)
{
	if(freePolylines){
		for(int i=0; i<set->count; i++){
			cpPolylineFree(set->lines[i]);
		}
	}
	
	free(set->lines);
}


void
cpPolylineSetFree(cpPolylineSet *set, bool freePolylines)
{
	if(set){
		cpPolylineSetDestroy(set, freePolylines);
		free(set);
	}
}

// Find the polyline that ends with v.
static int
cpPolylineSetFindEnds(cpPolylineSet *set, phy_vect v){
	int count = set->count;
	cpPolyline **lines = set->lines;
	
  for(int i=0; i<count; i++){
		cpPolyline *line = lines[i];
    if(cpveql(line->verts[line->count - 1], v)) return i;
  }
  
  return -1;
}

// Find the polyline that starts with v.
static int
cpPolylineSetFindStarts(cpPolylineSet *set, phy_vect v){
	int count = set->count;
	cpPolyline **lines = set->lines;
	
  for(int i=0; i<count; i++){
    if(cpveql(lines[i]->verts[0], v)) return i;
  }
  
  return -1;
}

// Add a new polyline to a polyline set.
static void
cpPolylineSetPush(cpPolylineSet *set, cpPolyline *line)
{
  // grow set
  set->count++;
  if(set->count > set->capacity){
    set->capacity *= 2;
    set->lines = (cpPolyline**)realloc(set->lines, set->capacity*sizeof(cpPolyline));
  }
  
	set->lines[set->count - 1] = line;
}

// Add a new polyline to a polyline set.
static void
cpPolylineSetAdd(cpPolylineSet *set, phy_vect v0, phy_vect v1)
{
	cpPolylineSetPush(set, cpPolylineMake2(DEFAULT_POLYLINE_CAPACITY, v0, v1));
}

// Join two cpPolylines in a polyline set together.
static void
cpPolylineSetJoin(cpPolylineSet *set, int before, int after)
{
  cpPolyline *lbefore = set->lines[before];
  cpPolyline *lafter = set->lines[after];
  
  // append
  int count = lbefore->count;
  lbefore = cpPolylineGrow(lbefore, lafter->count);
  memmove(lbefore->verts + count, lafter->verts, lafter->count*sizeof(phy_vect));
	set->lines[before] = lbefore;
  
  // delete lafter
  set->count--;
	cpPolylineFree(set->lines[after]);
  set->lines[after] = set->lines[set->count];
}

// Add a segment to a polyline set.
// A segment will either start a new polyline, join two others, or add to or loop an existing polyline.
void
cpPolylineSetCollectSegment(phy_vect v0, phy_vect v1, cpPolylineSet *lines)
{
  int before = cpPolylineSetFindEnds(lines, v0);
  int after = cpPolylineSetFindStarts(lines, v1);
  
  if(before >= 0 && after >= 0){
    if(before == after){
      // loop by pushing v1 onto before
      lines->lines[before] = cpPolylinePush(lines->lines[before], v1);
    } else {
      // join before and after
      cpPolylineSetJoin(lines, before, after);
    }
  } else if(before >= 0){
    // push v1 onto before
    lines->lines[before] = cpPolylinePush(lines->lines[before], v1);
  } else if(after >= 0){
    // enqueue v0 onto after
    lines->lines[after] = cpPolylineEnqueue(lines->lines[after], v0);
  } else {
    // create new line from v0 and v1
    cpPolylineSetAdd(lines, v0, v1);
  }
}

//MARK: Convex Hull Functions

cpPolyline *
cpPolylineToConvexHull(cpPolyline *line, float tol)
{
	cpPolyline *hull = cpPolylineMake(line->count + 1);
	hull->count = cpConvexHull(line->count, line->verts, hull->verts, NULL, tol);
	hull = cpPolylinePush(hull, hull->verts[0]);
	
	return cpPolylineShrink(hull);
}

//MARK: Approximate Concave Decompostition

struct Notch {
	int i;
	float d;
	phy_vect v;
	phy_vect n;
};

static float
FindSteiner(int count, phy_vect *verts, struct Notch notch)
{
	float min = INFINITY;
	float feature = -1.0;
	
	for(int i=1; i<count-1; i++){
		int index = (notch.i + i)%count;
		
		phy_vect seg_a = verts[index];
		phy_vect seg_b = verts[Next(index, count)];
		
		float thing_a = cpvcross(notch.n, cpvsub(seg_a, notch.v));
		float thing_b = cpvcross(notch.n, cpvsub(seg_b, notch.v));
		if(thing_a*thing_b <= 0.0){
			float t = thing_a/(thing_a - thing_b);
			float dist = cpvdot(notch.n, cpvsub(cpvlerp(seg_a, seg_b, t), notch.v));
			
			if(dist >= 0.0 && dist <= min){
				min = dist;
				feature = index + t;
			}
		}
	}
	
	return feature;
}

//static float
//FindSteiner2(phy_vect *verts, int count, struct Notch notch)
//{
//	phy_vect a = verts[(notch.i + count - 1)%count];
//	phy_vect b = verts[(notch.i + 1)%count];
//	phy_vect n = cpvnormalize(cpvadd(cpvnormalize(cpvsub(notch.v, a)), cpvnormalize(cpvsub(notch.v, b))));
//	
//	float min = INFINITY;
//	float feature = -1.0;
//	
//	for(int i=1; i<count-1; i++){
//		int index = (notch.i + i)%count;
//		
//		phy_vect seg_a = verts[index];
//		phy_vect seg_b = verts[Next(index, count)];
//		
//		float thing_a = cpvcross(n, cpvsub(seg_a, notch.v));
//		float thing_b = cpvcross(n, cpvsub(seg_b, notch.v));
//		if(thing_a*thing_b <= 0.0){
//			float t = thing_a/(thing_a - thing_b);
//			float dist = cpvdot(n, cpvsub(cpvlerp(seg_a, seg_b, t), notch.v));
//			
//			if(dist >= 0.0 && dist <= min){
//				min = dist;
//				feature = index + t;
//			}
//		}
//	}
//	
//	cpAssertSoft(feature >= 0.0, "No closest features detected. This is likely due to a self intersecting polygon.");
//	return feature;
//}

//struct Range {float min, max;};
//static inline struct Range
//clip_range(phy_vect delta_a, phy_vect delta_b, phy_vect clip)
//{
//	float da = cpvcross(delta_a, clip);
//	float db = cpvcross(delta_b, clip);
//	float clamp = da/(da - db);
//	if(da > db){
//		return (struct Range){-INFINITY, clamp};
//	} else if(da < db){
//		return (struct Range){clamp, INFINITY};
//	} else {
//		return (struct Range){-INFINITY, INFINITY};
//	}
//}
//
//static float
//FindSteiner3(phy_vect *verts, int count, struct Notch notch)
//{
//	float min = INFINITY;
//	float feature = -1.0;
//	
//	phy_vect support_a = verts[(notch.i - 1 + count)%count];
//	phy_vect support_b = verts[(notch.i + 1)%count];
//	
//	phy_vect clip_a = cpvlerp(support_a, support_b, 0.1);
//	phy_vect clip_b = cpvlerp(support_b, support_b, 0.9);
//	
//	for(int i=1; i<count - 1; i++){
//		int index = (notch.i + i)%count;
//		phy_vect seg_a = verts[index];
//		phy_vect seg_b = verts[Next(index, count)];
//		
//		phy_vect delta_a = cpvsub(seg_a, notch.v);
//		phy_vect delta_b = cpvsub(seg_b, notch.v);
//		
//		// Ignore if the segment faces away from the point.
//		if(cpvcross(delta_b, delta_a) > 0.0){
//			struct Range range1 = clip_range(delta_a, delta_b, cpvsub(notch.v, clip_a));
//			struct Range range2 = clip_range(delta_a, delta_b, cpvsub(clip_b, notch.v));
//			
//			float min_t = cpfmax(0.0, cpfmax(range1.min, range2.min));
//			float max_t = cpfmin(1.0, cpfmin(range1.max, range2.max));
//			
//			// Ignore if the segment has been completely clipped away.
//			if(min_t < max_t){
//				phy_vect seg_delta = cpvsub(seg_b, seg_a);
//				float closest_t = cpfclamp(cpvdot(seg_delta, cpvsub(notch.v, seg_a))/cpvlengthsq(seg_delta), min_t, max_t);
//				phy_vect closest = cpvlerp(seg_a, seg_b, closest_t);
//				
//				float dist = cpvdistsq(notch.v, closest);
//				if(dist < min){
//					min = dist;
//					feature = index + closest_t;
//				}
//			}
//		}
//	}
//	
//	cpAssertWarn(feature >= 0.0, "Internal Error: No closest features detected.");
//	return feature;
//}

//static bool
//VertexUnobscured(int count, phy_vect *verts, int index, int notch_i)
//{
//	phy_vect v = verts[notch_i];
//	phy_vect n = cpvnormalize(cpvsub(verts[index], v));
//	
//	for(int i=0; i<count; i++){
//		if(i == index || i == Next(i, count) || i == notch_i || i == Next(notch_i, count)) continue;
//		
//		phy_vect seg_a = verts[i];
//		phy_vect seg_b = verts[Next(i, count)];
//		
//		float thing_a = cpvcross(n, cpvsub(seg_a, v));
//		float thing_b = cpvcross(n, cpvsub(seg_b, v));
//		if(thing_a*thing_b <= 0.0) return true;
//	}
//	
//	return false;
//}
//
//static float
//FindSteiner4(int count, phy_vect *verts, struct Notch notch, float *convexity)
//{
//	float min = INFINITY;
//	float feature = -1.0;
//	
//	for(int i=Next(notch.b, count); i!=notch.a; i=Next(i, count)){
//		phy_vect v = verts[i];
//		float weight = (1.0 + 0.1*convexity[i])/(1.0*cpvdist(notch.v, v));
//		
//		if(weight <= min && VertexUnobscured(count, verts, i, notch.i)){
//			min = weight;
//			feature = i;
//		}
//	}
//	
//	cpAssertSoft(feature >= 0.0, "No closest features detected. This is likely due to a self intersecting polygon.");
//	return feature;
//}

static struct Notch
DeepestNotch(int count, phy_vect *verts, int hullCount, phy_vect *hullVerts, int first, float tol)
{
	struct Notch notch = {};
	int j = Next(first, count);
	
	for(int i=0; i<hullCount; i++){
		phy_vect a = hullVerts[i];
		phy_vect b = hullVerts[Next(i, hullCount)];
		
		// TODO use a cross check instead?
		phy_vect n = cpvnormalize(cpvrperp(cpvsub(a, b)));
		float d = cpvdot(n, a);
		
		phy_vect v = verts[j];
		while(!cpveql(v, b)){
			float depth = cpvdot(n, v) - d;
			
			if(depth > notch.d){
				notch.d = depth;
				notch.i = j;
				notch.v = v;
				notch.n = n;
			}
			
			j = Next(j, count);
			v = verts[j];
		}
		
		j = Next(j, count);
	}
	
	return notch;
}

static inline int IMAX(int a, int b){return (a > b ? a : b);}

static void
ApproximateConcaveDecomposition(phy_vect *verts, int count, float tol, cpPolylineSet *set)
{
	int first;
	phy_vect *hullVerts = (phy_vect*) alloca(count*sizeof(phy_vect));
	int hullCount = cpConvexHull(count, verts, hullVerts, &first, 0.0);
	
	if(hullCount != count){
		struct Notch notch = DeepestNotch(count, verts, hullCount, hullVerts, first, tol);
		
		if(notch.d > tol){
			float steiner_it = FindSteiner(count, verts, notch);
			
			if(steiner_it >= 0.0){
				int steiner_i = (int)steiner_it;
				phy_vect steiner = cpvlerp(verts[steiner_i], verts[Next(steiner_i, count)], steiner_it - steiner_i);
				
				// Vertex counts NOT including the steiner point.
				int sub1_count = (steiner_i - notch.i + count)%count + 1;
				int sub2_count = count - (steiner_i - notch.i + count)%count;
				phy_vect *scratch = (phy_vect*) alloca((IMAX(sub1_count, sub2_count) + 1)*sizeof(phy_vect));
				
				for(int i=0; i<sub1_count; i++) scratch[i] = verts[(notch.i + i)%count];
				scratch[sub1_count] = steiner;
				ApproximateConcaveDecomposition(scratch, sub1_count + 1, tol, set);
				
				for(int i=0; i<sub2_count; i++) scratch[i] = verts[(steiner_i + 1 + i)%count];
				scratch[sub2_count] = steiner;
				ApproximateConcaveDecomposition(scratch, sub2_count + 1, tol, set);
				
				return;
			}
		}
	}
	
	cpPolyline *hull = cpPolylineMake(hullCount + 1);
	
	memcpy(hull->verts, hullVerts, hullCount*sizeof(phy_vect));
	hull->verts[hullCount] = hullVerts[0];
	hull->count = hullCount + 1;
	
	cpPolylineSetPush(set, hull);
}

cpPolylineSet *
cpPolylineConvexDecomposition_BETA(cpPolyline *line, float tol)
{
	cpAssertSoft(cpPolylineIsClosed(line), "Cannot decompose an open polygon.");
	cpAssertSoft(cpAreaForPoly(line->count, line->verts, 0.0) >= 0.0, "Winding is backwards. (Are you passing a hole?)");
	
	cpPolylineSet *set = cpPolylineSetNew();
	ApproximateConcaveDecomposition(line->verts, line->count - 1, tol, set);
	
	return set;
}
