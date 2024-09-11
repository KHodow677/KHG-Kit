#include "entity/indicators.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_gfx/elements.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

void generate_all_indicators(selector_info *info, physics_info *p_info, renderer_info *r_info, mover_info *m_info) {
  utl_vector_clear(r_info->indicators);
  for (int i = 0; i < utl_queue_size(m_info->target_pos_queue); i++) {
    phy_vect *point_pos = utl_vector_at(m_info->target_pos_queue->vec, i);
    generate_indicator(r_info, INDICATOR_POINT, POINT, false, false, *point_pos);
  }
  generate_indicator(r_info, INDICATOR_OUTLINE, TANK_BODY_OUTLINE, true, true, phy_v(0.0f, 0.0f));
  generate_indicator(r_info, INDICATOR_OUTLINE, TANK_TOP_OUTLINE, true, false, phy_v(0.0f, 0.0f));
}

void generate_indicator(renderer_info *r_info, indicator_type type, int tex, bool follow, bool is_target_body, phy_vect pos) {
  indicator ind = { type, follow, is_target_body, tex, pos };
  utl_vector_push_back(r_info->indicators, &ind);
}

void render_outline(renderer_info *info, physics_info *p_info, indicator *ind) {
  phy_vect pos = ind->is_target_body ? phy_body_get_position(p_info->target_body) : phy_body_get_position(p_info->body);
  float angle = ind->is_target_body ? phy_body_get_angle(p_info->target_body) : phy_body_get_angle(p_info->body);
  gfx_texture *tex = utl_vector_at(TEXTURE_LOOKUP, ind->tex_id);
  tex->angle = angle;
  gfx_image_no_block(pos.x, pos.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
}

void render_point(indicator *ind) {
  gfx_texture *tex = utl_vector_at(TEXTURE_LOOKUP, ind->tex_id);
  gfx_image_no_block(ind->pos.x, ind->pos.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
}

