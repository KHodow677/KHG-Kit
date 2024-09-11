#include "entity/indicators.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_phy/body.h"

void generate_indicator(renderer_info *r_info, indicator_type type, int tex, phy_vect pos, float ang, bool follow, bool is_target_body) {
  indicator ind = { type, follow, is_target_body, tex, pos, ang };
  utl_vector_push_back(r_info->indicators, &ind);
}

void render_outline(renderer_info *info, physics_info *p_info, indicator *ind) {
  phy_vect pos = ind->is_target_body ? phy_body_get_position(p_info->target_body) : phy_body_get_position(p_info->body);
  float angle = ind->is_target_body ? phy_body_get_angle(p_info->target_body) : phy_body_get_angle(p_info->body);
  if (ind->follow) {
    ind->pos = pos;
    ind->ang = angle;
  }
  gfx_texture *tex = utl_vector_at(TEXTURE_LOOKUP, ind->tex_id);
  tex->angle = ind->ang;
  gfx_image_no_block(ind->pos.x, ind->pos.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
}

void render_point(indicator *ind) {
  gfx_texture *tex = utl_vector_at(TEXTURE_LOOKUP, ind->tex_id);
  gfx_image_no_block(ind->pos.x, ind->pos.y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
}

