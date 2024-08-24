#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

typedef float (*phy_march_sample_func)(phy_vect point, void *data);
typedef void (*phy_march_segment_func)(phy_vect v0, phy_vect v1, void *data);

void phy_march_soft(phy_bb bb, unsigned long x_samples, unsigned long y_samples, float threshold, phy_march_segment_func segment, void *segment_data, phy_march_sample_func sample, void *sample_data);
void phy_march_hard(phy_bb bb, unsigned long x_samples, unsigned long y_samples, float threshold, phy_march_segment_func segment, void *segment_data, phy_march_sample_func sample, void *sample_data);

