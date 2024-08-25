#include "khg_phy/phy.h"
#include "khg_phy/phy_private.h"
#include "khg_phy/polyline.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static inline int Next(int i, int count){return (i+1)%count;}

//MARK: Polylines

#define DEFAULT_POLYLINE_CAPACITY 16

static int
cpPolylineSizeForCapacity(int capacity)
{
	return sizeof(phy_polyline) + capacity*sizeof(phy_vect);
}

static phy_polyline *
cpPolylineMake(int capacity)
{
	capacity = (capacity > DEFAULT_POLYLINE_CAPACITY ? capacity : DEFAULT_POLYLINE_CAPACITY);
	
	phy_polyline *line = (phy_polyline *)calloc(1, cpPolylineSizeForCapacity(capacity));
	line->count = 0;
	line->capacity = capacity;
	
	return line;
}

static phy_polyline *
cpPolylineMake2(int capacity, phy_vect a, phy_vect b)
{
	phy_polyline *line = cpPolylineMake(capacity);
	line->count = 2;
	line->verts[0] = a;
	line->verts[1] = b;
	
	return line;
}

static phy_polyline *
cpPolylineShrink(phy_polyline *line)
{
	line->capacity = line->count;
	return (phy_polyline*)realloc(line, cpPolylineSizeForCapacity(line->count));
}

void
phy_polyline_free(phy_polyline *line)
{
	free(line);
}

// Grow the allocated memory for a polyline.
static phy_polyline *
cpPolylineGrow(phy_polyline *line, int count)
{
  line->count += count;
  
  int capacity = line->capacity;
  while(line->count > capacity) capacity *= 2;
  
  if(line->capacity < capacity){
    line->capacity = capacity;
		line = (phy_polyline*)realloc(line, cpPolylineSizeForCapacity(capacity));
  }
	
	return line;
}

// Push v onto the end of line.
static phy_polyline *
cpPolylinePush(phy_polyline *line, phy_vect v)
{
  int count = line->count;
  line = cpPolylineGrow(line, 1);
  line->verts[count] = v;
	
	return line;
}

// Push v onto the beginning of line.
static phy_polyline *
cpPolylineEnqueue(phy_polyline *line, phy_vect v)
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
phy_polyline_is_closed(phy_polyline *line)
{
	return (line->count > 1 && phy_v_eql(line->verts[0], line->verts[line->count-1]));
}

// Check if a cpPolyline is longer than a certain length
// Takes a range which can wrap around if the polyline is looped.
static bool
cpPolylineIsShort(phy_vect *points, int count, int start, int end, float min)
{
  float length = 0.0f;
	for(int i=start; i!=end; i=Next(i, count)){
		length += phy_v_dist(points[i], points[Next(i, count)]);
		if(length > min) return false;
	}
  
  return true;
}

//MARK: Polyline Simplification

static inline float
Sharpness(phy_vect a, phy_vect b, phy_vect c)
{
	// TODO could speed this up by caching the normals instead of calculating each twice.
  return phy_v_dot(phy_v_normalize(phy_v_sub(a, b)), phy_v_normalize(phy_v_sub(c, b)));
}

// Join similar adjacent line segments together. Works well for hard edged shapes.
// 'tol' is the minimum anglular difference in radians of a vertex.
phy_polyline *
phy_polyline_simplify_vertices(phy_polyline *line, float tol)
{
	phy_polyline *reduced = cpPolylineMake2(0, line->verts[0], line->verts[1]);
	
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
		phy_polyline_is_closed(line) &&
		Sharpness(reduced->verts[reduced->count - 2], reduced->verts[0], reduced->verts[1]) < minSharp
	){
		reduced->verts[0] = reduced->verts[reduced->count - 2];
		reduced->count--;
	}
	
	// TODO shrink
	return reduced;
}

// Recursive function used by cpPolylineSimplifyCurves().
static phy_polyline *
DouglasPeucker(
	phy_vect *verts, phy_polyline *reduced,
	int length, int start, int end,
	float min, float tol
){
	// Early exit if the points are adjacent
  if((end - start + length)%length < 2) return reduced;
  
	phy_vect a = verts[start];
	phy_vect b = verts[end];
	
	// Check if the length is below the threshold
	if(phy_v_near(a, b, min) && cpPolylineIsShort(verts, length, start, end, min)) return reduced;
	
	// Find the maximal vertex to split and recurse on
	float max = 0.0;
	int maxi = start;
	
	phy_vect n = phy_v_normalize(phy_v_perp(phy_v_sub(b, a)));
	float d = phy_v_dot(n, a);
	
	for(int i=Next(start, length); i!=end; i=Next(i, length)){
		float dist = fabs(phy_v_dot(n, verts[i]) - d);
		
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
phy_polyline *
phy_polyline_simplify_curves(phy_polyline *line, float tol)
{
	phy_polyline *reduced = cpPolylineMake(line->count);
	
	float min = tol/2.0f;
  
  if(phy_polyline_is_closed(line)){
		int start, end;
    cp_loop_indexes(line->verts, line->count - 1, &start, &end);
    
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

phy_polyline_set *
phy_polyline_set_alloc(void)
{
	return (phy_polyline_set *)calloc(1, sizeof(phy_polyline_set));
}

phy_polyline_set *
phy_polyline_set_init(phy_polyline_set *set)
{
	set->count = 0;
	set->capacity = 8;
	set->lines = (phy_polyline**)calloc(set->capacity, sizeof(phy_polyline));
	
  return set;
}


phy_polyline_set *
phy_polyline_set_new(void)
{
	return phy_polyline_set_init(phy_polyline_set_alloc());
}

void
phy_polyline_set_destroy(phy_polyline_set *set, bool freePolylines)
{
	if(freePolylines){
		for(int i=0; i<set->count; i++){
			phy_polyline_free(set->lines[i]);
		}
	}
	
	free(set->lines);
}


void
phy_polyline_set_free(phy_polyline_set *set, bool freePolylines)
{
	if(set){
		phy_polyline_set_destroy(set, freePolylines);
		free(set);
	}
}

// Find the polyline that ends with v.
static int
cpPolylineSetFindEnds(phy_polyline_set *set, phy_vect v){
	int count = set->count;
	phy_polyline **lines = set->lines;
	
  for(int i=0; i<count; i++){
		phy_polyline *line = lines[i];
    if(phy_v_eql(line->verts[line->count - 1], v)) return i;
  }
  
  return -1;
}

// Find the polyline that starts with v.
static int
cpPolylineSetFindStarts(phy_polyline_set *set, phy_vect v){
	int count = set->count;
	phy_polyline **lines = set->lines;
	
  for(int i=0; i<count; i++){
    if(phy_v_eql(lines[i]->verts[0], v)) return i;
  }
  
  return -1;
}

// Add a new polyline to a polyline set.
static void
cpPolylineSetPush(phy_polyline_set *set, phy_polyline *line)
{
  // grow set
  set->count++;
  if(set->count > set->capacity){
    set->capacity *= 2;
    set->lines = (phy_polyline**)realloc(set->lines, set->capacity*sizeof(phy_polyline));
  }
  
	set->lines[set->count - 1] = line;
}

// Add a new polyline to a polyline set.
static void
cpPolylineSetAdd(phy_polyline_set *set, phy_vect v0, phy_vect v1)
{
	cpPolylineSetPush(set, cpPolylineMake2(DEFAULT_POLYLINE_CAPACITY, v0, v1));
}

// Join two cpPolylines in a polyline set together.
static void
cpPolylineSetJoin(phy_polyline_set *set, int before, int after)
{
  phy_polyline *lbefore = set->lines[before];
  phy_polyline *lafter = set->lines[after];
  
  // append
  int count = lbefore->count;
  lbefore = cpPolylineGrow(lbefore, lafter->count);
  memmove(lbefore->verts + count, lafter->verts, lafter->count*sizeof(phy_vect));
	set->lines[before] = lbefore;
  
  // delete lafter
  set->count--;
	phy_polyline_free(set->lines[after]);
  set->lines[after] = set->lines[set->count];
}

// Add a segment to a polyline set.
// A segment will either start a new polyline, join two others, or add to or loop an existing polyline.
void
phy_polyline_set_collect_segment(phy_vect v0, phy_vect v1, phy_polyline_set *lines)
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

phy_polyline *
phy_polyline_to_convex_hull(phy_polyline *line, float tol)
{
	phy_polyline *hull = cpPolylineMake(line->count + 1);
	hull->count = phy_convex_hull(line->count, line->verts, hull->verts, NULL, tol);
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
		
		float thing_a = phy_v_cross(notch.n, phy_v_sub(seg_a, notch.v));
		float thing_b = phy_v_cross(notch.n, phy_v_sub(seg_b, notch.v));
		if(thing_a*thing_b <= 0.0){
			float t = thing_a/(thing_a - thing_b);
			float dist = phy_v_dot(notch.n, phy_v_sub(phy_v_lerp(seg_a, seg_b, t), notch.v));
			
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
//	if (feature < 0.0) {
//	  utl_error_func("No closest features detected, this is likely due to a self intersecting polygon", utl_user_defined_data);
//	}
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
//	if (feature < 0.0) {
//	  utl_error_func("No closest features detected", utl_user_defined_data);
//	}
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
//	if (feature < 0.0) {
//	  utl_error_func("No closest features detected, this is likely due to a self intersecting polygon", utl_user_defined_data);
//	}
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
		phy_vect n = phy_v_normalize(phy_v_rperp(phy_v_sub(a, b)));
		float d = phy_v_dot(n, a);
		
		phy_vect v = verts[j];
		while(!phy_v_eql(v, b)){
			float depth = phy_v_dot(n, v) - d;
			
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
ApproximateConcaveDecomposition(phy_vect *verts, int count, float tol, phy_polyline_set *set)
{
	int first;
	phy_vect *hullVerts = (phy_vect*) alloca(count*sizeof(phy_vect));
	int hullCount = phy_convex_hull(count, verts, hullVerts, &first, 0.0);
	
	if(hullCount != count){
		struct Notch notch = DeepestNotch(count, verts, hullCount, hullVerts, first, tol);
		
		if(notch.d > tol){
			float steiner_it = FindSteiner(count, verts, notch);
			
			if(steiner_it >= 0.0){
				int steiner_i = (int)steiner_it;
				phy_vect steiner = phy_v_lerp(verts[steiner_i], verts[Next(steiner_i, count)], steiner_it - steiner_i);
				
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
	
	phy_polyline *hull = cpPolylineMake(hullCount + 1);
	
	memcpy(hull->verts, hullVerts, hullCount*sizeof(phy_vect));
	hull->verts[hullCount] = hullVerts[0];
	hull->count = hullCount + 1;
	
	cpPolylineSetPush(set, hull);
}

phy_polyline_set *
cpPolylineConvexDecomposition_BETA(phy_polyline *line, float tol)
{
	if (!phy_polyline_is_closed(line)) {
    utl_error_func("Cannot decompose an open polygon", utl_user_defined_data);
  }
	if (phy_area_for_poly(line->count, line->verts, 0.0) < 0.0) {
    utl_error_func("Winding is backwards, likely because you are passing a hole", utl_user_defined_data);
  }
	
	phy_polyline_set *set = phy_polyline_set_new();
	ApproximateConcaveDecomposition(line->verts, line->count - 1, tol, set);
	
	return set;
}
