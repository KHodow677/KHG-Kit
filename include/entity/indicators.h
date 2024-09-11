#pragma once

#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_selector.h"
#include "khg_phy/phy_types.h"
#include <stdbool.h>

typedef enum {
  INDICATOR_OUTLINE,
  INDICATOR_POINT,
  INDICATOR_LINE,
  INDICATOR_BODY_LINE
} indicator_type;

typedef struct indicator {
  indicator_type type;
  bool is_target_body;
  int tex_id;
  phy_vect pos;
  float ang;
  float length;
} indicator;

void generate_all_indicators(selector_info *info, physics_info *p_info, renderer_info *r_info, mover_info *m_info);
void generate_indicator(renderer_info *r_info, indicator_type type, int tex, bool is_target_body, phy_vect pos, float ang, float length);

void render_outline(renderer_info *info, physics_info *p_info, indicator *ind);
void render_point(indicator *ind);
void render_line(indicator *ind);
void render_body_line(renderer_info *info, physics_info *p_info, indicator *ind);
