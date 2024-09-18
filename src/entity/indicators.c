#include "entity/indicators.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_gfx/elements.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"

void generate_all_indicators(selector_info *info, physics_info *p_info, renderer_info *r_info, mover_info *m_info) {
  utl_vector_clear(r_info->indicators);
  if (!utl_queue_empty(m_info->target_pos_queue)) {
    phy_vect *first_point_pos = utl_queue_front(m_info->target_pos_queue);
    generate_indicator(r_info, INDICATOR_BODY_LINE, COMMAND_LINE, true, *first_point_pos, 0.0f, 0.0f);
    for (int i = 0; i < utl_queue_size(m_info->target_pos_queue) - 1; i++) {
      phy_vect *point_pos_1 = utl_vector_at(m_info->target_pos_queue->vec, i);
      phy_vect *point_pos_2 = utl_vector_at(m_info->target_pos_queue->vec, i + 1);
      phy_vect mid = phy_v((point_pos_1->x + point_pos_2->x) * 0.5f, (point_pos_1->y + point_pos_2->y) * 0.5f);
      float ang = normalize_angle(atan2f(point_pos_2->y - point_pos_1->y, point_pos_2->x - point_pos_1->x) - M_PI / 2);
      float len = phy_v_dist(*point_pos_1, *point_pos_2);
      generate_indicator(r_info, INDICATOR_LINE, COMMAND_LINE, false, mid, ang, len);
    }
    for (int i = 0; i < utl_queue_size(m_info->target_pos_queue); i++) {
      phy_vect *point_pos = utl_vector_at(m_info->target_pos_queue->vec, i);
      generate_indicator(r_info, INDICATOR_POINT, COMMAND_POINT, false, *point_pos, 0.0f, 0.0f);
    }
  }
  generate_indicator(r_info, INDICATOR_OUTLINE, TANK_BODY_OUTLINE, true, phy_v(0.0f, 0.0f), 0.0f, 0.0f);
  generate_indicator(r_info, INDICATOR_OUTLINE, TANK_TOP_OUTLINE, false, phy_v(0.0f, 0.0f), 0.0f, 0.0f);
}

void generate_indicator(renderer_info *r_info, indicator_type type, int tex, bool is_target_body, phy_vect pos, float ang, float length) {
  indicator ind = { type, is_target_body, tex, pos, ang, length};
  utl_vector_push_back(r_info->indicators, &ind);
}

void render_outline(renderer_info *info, physics_info *p_info, indicator *ind) {
  phy_vect pos = ind->is_target_body ? phy_body_get_position(p_info->target_body) : phy_body_get_position(p_info->body);
  float angle = ind->is_target_body ? phy_body_get_angle(p_info->target_body) : phy_body_get_angle(p_info->body);
  gfx_texture *tex = get_or_add_texture(ind->tex_id);
  tex->angle = angle;
  gfx_image_no_block(pos.x, pos.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom, true);
}

void render_point(indicator *ind) {
  gfx_texture *tex = get_or_add_texture(ind->tex_id);
  gfx_image_no_block(ind->pos.x, ind->pos.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom, true);
}

void render_line(indicator *ind) {
  gfx_texture *tex = get_or_add_texture(ind->tex_id);
  tex->angle = ind->ang;
  tex->height = ind->length;
  gfx_image_no_block(ind->pos.x, ind->pos.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom, false);
}

void render_body_line(renderer_info *info, physics_info *p_info, indicator *ind) {
  phy_vect pos = ind->is_target_body ? phy_body_get_position(p_info->target_body) : phy_body_get_position(p_info->body);
  float ang = normalize_angle(atan2f(pos.y - ind->pos.y, pos.x - ind->pos.x) - M_PI / 2);
  phy_vect mid = phy_v((pos.x + ind->pos.x) * 0.5f, (pos.y + ind->pos.y) * 0.5f);
  float len = phy_v_dist(pos, ind->pos);
  gfx_texture *tex = get_or_add_texture(ind->tex_id);
  tex->angle = ang;
  tex->height = len;
  gfx_image_no_block(mid.x, mid.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom, false);
}
