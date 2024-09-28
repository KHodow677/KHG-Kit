#pragma once

#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_selector.h"
#include "khg_phy/phy_types.h"
#include <stdbool.h>

typedef enum {
  INDICATOR_POINT,
  INDICATOR_LINE,
  INDICATOR_BODY_POINT,
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

void generate_all_indicators(selector_info *info, comp_physics *p_info, comp_renderer *r_info, comp_mover *m_info);
void generate_indicator(comp_renderer *r_info, indicator_type type, int tex, bool is_target_body, phy_vect pos, float ang, float length);

void render_point(indicator *ind);
void render_line(indicator *ind);
void render_body_point(comp_physics *p_info, indicator *ind);
void render_body_line(comp_physics *p_info, indicator *ind);
