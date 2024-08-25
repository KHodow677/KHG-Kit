// Copyright 2013 Howling Moon Software. All rights reserved.
// See http://chipmunk2d.net/legal.php for more information.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "khg_phy/phy.h"
#include "khg_phy/march.h"


typedef void (*cpMarchCellFunc)(
	float t, float a, float b, float c, float d,
	float x0, float x1, float y0, float y1,
	phy_march_segment_func segment, void *segment_data
);

// The looping and sample caching code is shared between cpMarchHard() and cpMarchSoft().
static void
cpMarchCells(
  phy_bb bb, unsigned long x_samples, unsigned long y_samples, float t,
  phy_march_segment_func segment, void *segment_data,
  phy_march_sample_func sample, void *sample_data,
	cpMarchCellFunc cell
){
	float x_denom = 1.0/(float)(x_samples - 1);
	float y_denom = 1.0/(float)(y_samples - 1);
	
	// TODO range assertions and short circuit for 0 sized windows.
	
	// Keep a copy of the previous row to avoid double lookups.
	float *buffer = (float *)calloc(x_samples, sizeof(float));
	for(unsigned long i=0; i<x_samples; i++) buffer[i] = sample(phy_v(phy_lerp(bb.l, bb.r, i*x_denom), bb.b), sample_data);
	
	for(unsigned long j=0; j<y_samples-1; j++){
		float y0 = phy_lerp(bb.b, bb.t, (j+0)*y_denom);
		float y1 = phy_lerp(bb.b, bb.t, (j+1)*y_denom);
		
		float a, b = buffer[0];
		float c, d = sample(phy_v(bb.l, y1), sample_data);
		buffer[0] = d;
		
		for(unsigned long i=0; i<x_samples-1; i++){
			float x0 = phy_lerp(bb.l, bb.r, (i+0)*x_denom);
			float x1 = phy_lerp(bb.l, bb.r, (i+1)*x_denom);
			
			a = b; b = buffer[i + 1];
			c = d; d = sample(phy_v(x1, y1), sample_data);
			buffer[i + 1] = d;
			
			cell(t, a, b, c, d, x0, x1, y0, y1, segment, segment_data);
		}
	}
	
	free(buffer);
}


// TODO should flip this around eventually.
static inline void
seg(phy_vect v0, phy_vect v1, phy_march_segment_func f, void *data)
{
	if(!phy_v_eql(v0, v1)) f(v1, v0, data);
}

// Lerps between two positions based on their sample values.
static inline float
midlerp(float x0, float x1, float s0, float s1, float t)
{
	return phy_lerp(x0, x1, (t - s0)/(s1 - s0));
}

static void
cpMarchCellSoft(
	float t, float a, float b, float c, float d,
	float x0, float x1, float y0, float y1,
	phy_march_segment_func segment, void *segment_data
){
	// TODO this switch part is super expensive, can it be NEONized?
	switch((a>t)<<0 | (b>t)<<1 | (c>t)<<2 | (d>t)<<3){
		case 0x1: seg(phy_v(x0, midlerp(y0,y1,a,c,t)), phy_v(midlerp(x0,x1,a,b,t), y0), segment, segment_data); break;
		case 0x2: seg(phy_v(midlerp(x0,x1,a,b,t), y0), phy_v(x1, midlerp(y0,y1,b,d,t)), segment, segment_data); break;
		case 0x3: seg(phy_v(x0, midlerp(y0,y1,a,c,t)), phy_v(x1, midlerp(y0,y1,b,d,t)), segment, segment_data); break;
		case 0x4: seg(phy_v(midlerp(x0,x1,c,d,t), y1), phy_v(x0, midlerp(y0,y1,a,c,t)), segment, segment_data); break;
		case 0x5: seg(phy_v(midlerp(x0,x1,c,d,t), y1), phy_v(midlerp(x0,x1,a,b,t), y0), segment, segment_data); break;
		case 0x6: seg(phy_v(midlerp(x0,x1,a,b,t), y0), phy_v(x1, midlerp(y0,y1,b,d,t)), segment, segment_data);
							seg(phy_v(midlerp(x0,x1,c,d,t), y1), phy_v(x0, midlerp(y0,y1,a,c,t)), segment, segment_data); break;
		case 0x7: seg(phy_v(midlerp(x0,x1,c,d,t), y1), phy_v(x1, midlerp(y0,y1,b,d,t)), segment, segment_data); break;
		case 0x8: seg(phy_v(x1, midlerp(y0,y1,b,d,t)), phy_v(midlerp(x0,x1,c,d,t), y1), segment, segment_data); break;
		case 0x9: seg(phy_v(x0, midlerp(y0,y1,a,c,t)), phy_v(midlerp(x0,x1,a,b,t), y0), segment, segment_data);
							seg(phy_v(x1, midlerp(y0,y1,b,d,t)), phy_v(midlerp(x0,x1,c,d,t), y1), segment, segment_data); break;
		case 0xA: seg(phy_v(midlerp(x0,x1,a,b,t), y0), phy_v(midlerp(x0,x1,c,d,t), y1), segment, segment_data); break;
		case 0xB: seg(phy_v(x0, midlerp(y0,y1,a,c,t)), phy_v(midlerp(x0,x1,c,d,t), y1), segment, segment_data); break;
		case 0xC: seg(phy_v(x1, midlerp(y0,y1,b,d,t)), phy_v(x0, midlerp(y0,y1,a,c,t)), segment, segment_data); break;
		case 0xD: seg(phy_v(x1, midlerp(y0,y1,b,d,t)), phy_v(midlerp(x0,x1,a,b,t), y0), segment, segment_data); break;
		case 0xE: seg(phy_v(midlerp(x0,x1,a,b,t), y0), phy_v(x0, midlerp(y0,y1,a,c,t)), segment, segment_data); break;
		default: break; // 0x0 and 0xF
	}
}

void
phy_march_soft(
  phy_bb bb, unsigned long x_samples, unsigned long y_samples, float t,
  phy_march_segment_func segment, void *segment_data,
  phy_march_sample_func sample, void *sample_data
){
	cpMarchCells(bb, x_samples, y_samples, t, segment, segment_data, sample, sample_data, cpMarchCellSoft);
}


// TODO should flip this around eventually.
static inline void
segs(phy_vect a, phy_vect b, phy_vect c, phy_march_segment_func f, void *data)
{
	seg(b, c, f, data);
	seg(a, b, f, data);
}

static void
cpMarchCellHard(
	float t, float a, float b, float c, float d,
	float x0, float x1, float y0, float y1,
	phy_march_segment_func segment, void *segment_data
){
	// midpoints
	float xm = phy_lerp(x0, x1, 0.5f);
	float ym = phy_lerp(y0, y1, 0.5f);
	
	switch((a>t)<<0 | (b>t)<<1 | (c>t)<<2 | (d>t)<<3){
		case 0x1: segs(phy_v(x0, ym), phy_v(xm, ym), phy_v(xm, y0), segment, segment_data); break;
		case 0x2: segs(phy_v(xm, y0), phy_v(xm, ym), phy_v(x1, ym), segment, segment_data); break;
		case 0x3: seg(phy_v(x0, ym), phy_v(x1, ym), segment, segment_data); break;
		case 0x4: segs(phy_v(xm, y1), phy_v(xm, ym), phy_v(x0, ym), segment, segment_data); break;
		case 0x5: seg(phy_v(xm, y1), phy_v(xm, y0), segment, segment_data); break;
		case 0x6: segs(phy_v(xm, y0), phy_v(xm, ym), phy_v(x0, ym), segment, segment_data);
		          segs(phy_v(xm, y1), phy_v(xm, ym), phy_v(x1, ym), segment, segment_data); break;
		case 0x7: segs(phy_v(xm, y1), phy_v(xm, ym), phy_v(x1, ym), segment, segment_data); break;
		case 0x8: segs(phy_v(x1, ym), phy_v(xm, ym), phy_v(xm, y1), segment, segment_data); break;
		case 0x9: segs(phy_v(x1, ym), phy_v(xm, ym), phy_v(xm, y0), segment, segment_data);
		          segs(phy_v(x0, ym), phy_v(xm, ym), phy_v(xm, y1), segment, segment_data); break;
		case 0xA: seg(phy_v(xm, y0), phy_v(xm, y1), segment, segment_data); break;
		case 0xB: segs(phy_v(x0, ym), phy_v(xm, ym), phy_v(xm, y1), segment, segment_data); break;
		case 0xC: seg(phy_v(x1, ym), phy_v(x0, ym), segment, segment_data); break;
		case 0xD: segs(phy_v(x1, ym), phy_v(xm, ym), phy_v(xm, y0), segment, segment_data); break;
		case 0xE: segs(phy_v(xm, y0), phy_v(xm, ym), phy_v(x0, ym), segment, segment_data); break;
		default: break; // 0x0 and 0xF
	}
}

void
phy_march_hard(
  phy_bb bb, unsigned long x_samples, unsigned long y_samples, float t,
  phy_march_segment_func segment, void *segment_data,
  phy_march_sample_func sample, void *sample_data
){
	cpMarchCells(bb, x_samples, y_samples, t, segment, segment_data, sample, sample_data, cpMarchCellHard);
}
