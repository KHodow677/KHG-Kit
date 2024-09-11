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
  INDICATOR_LINE
} indicator_type;

typedef struct indicator {
  indicator_type type;
  bool follow;
  bool is_target_body;
  int tex_id;
  phy_vect pos;
} indicator;

void generate_all_indicators(selector_info *info, physics_info *p_info, renderer_info *r_info, mover_info *m_info);
void generate_indicator(renderer_info *r_info, indicator_type type, int tex, bool follow, bool is_target_body, phy_vect pos);

void render_outline(renderer_info *info, physics_info *p_info, indicator *ind);
void render_point(indicator *ind);

