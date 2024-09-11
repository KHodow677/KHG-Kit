#pragma once

#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_phy/phy_types.h"
#include <stdbool.h>

typedef enum {
  INDICATOR_OUTLINE,
  INDICATOR_POINT,
  INDICATOR_LINE
} indicator_type;

typedef struct indicator {
  indicator_type type;
  bool follow;
  bool is_target_body;
  int tex_id;
  phy_vect pos;
  float ang;
} indicator;

void generate_indicator(renderer_info *r_info, indicator_type type, int tex, phy_vect pos, float ang, bool follow, bool is_target_body);

void render_outline(renderer_info *info, physics_info *p_info, indicator *ind);
void render_point(indicator *ind);

