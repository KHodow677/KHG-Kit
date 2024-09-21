#include "generators/components/comp_info_generator.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/indicators.h"
#include "entity/map.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "physics/physics_setup.h"
#include "khg_phy/body.h"
#include "khg_phy/constraint.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/pivot_joint.h"
#include "khg_phy/poly_shape.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"
#include <math.h>
#include <stdio.h>

void generate_physics_box(physics_info *info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog) {
  float moment = phy_moment_for_box(mass, width, height);
  info->body = phy_space_add_body(SPACE, phy_body_new(mass, moment));
  phy_body_set_position(info->body, pos);
  phy_body_set_center_of_gravity(info->body, cog);
  phy_body_set_angle(info->body, ang);
  if (collides) {
    info->shape = phy_space_add_shape(SPACE, phy_box_shape_new(info->body, width, height, 0.0f));
    phy_shape_set_friction(info->shape, 0.0f);
  }
  info->is_moving = false;
  info->is_turning = false;
  info->target_vel = 0.0f;
  info->target_ang_vel = 0.0f;
  info->move_enabled = true;
  info->rotate_enabled = true;
}

void generate_physics_pivot(physics_info *info, physics_info *p_info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog) {
  float moment = phy_moment_for_box(mass, width, height);
  info->body = phy_space_add_body(SPACE, phy_body_new(mass, moment));
  phy_body_set_position(info->body, pos);
  phy_body_set_center_of_gravity(info->body, cog);
  phy_body_set_angle(info->body, ang);
  if (collides) {
    info->shape = phy_space_add_shape(SPACE, phy_box_shape_new(info->body, width, height, 0.0f));
    phy_shape_set_friction(info->shape, 0.0f);
  }
  info->is_moving = false;
  info->is_turning = false;
  info->target_vel = 0.0f;
  info->target_ang_vel = 0.0f;
  info->move_enabled = false; 
  info->rotate_enabled = true; 
  info->target_body = p_info->body;
  if (p_info->shape) {
    info->target_shape = p_info->shape;
  }
  info->pivot = phy_space_add_constraint(SPACE, phy_pivot_joint_new_2(info->target_body, info->body, phy_body_get_center_of_gravity(p_info->body), phy_body_get_center_of_gravity(info->body)));
}

void generate_static_physics_circle(physics_info *info, bool collides, float radius, phy_vect pos, float ang, phy_vect cog) {
  info->body = phy_space_add_body(SPACE, phy_body_new_static());
  phy_body_set_position(info->body, pos);
  phy_body_set_center_of_gravity(info->body, cog);
  phy_body_set_angle(info->body, ang);
  if (collides) {
    info->shape = phy_space_add_shape(SPACE, phy_circle_shape_new(info->body, radius, phy_v(0.0f, 0.0f)));
    phy_shape_set_friction(info->shape, 0.0f);
  }
  info->is_moving = false;
  info->is_turning = false;
  info->target_vel = 0.0f;
  info->target_ang_vel = 0.0f;
  info->move_enabled = true;
  info->rotate_enabled = true;
}

void free_physics(physics_info *info, bool has_constraint) {
  if (has_constraint) {
    phy_space_remove_constraint(SPACE, info->pivot);
    phy_constraint_free(info->pivot);
  }
  if (info->shape) {
    phy_space_remove_shape(SPACE, info->shape);
    phy_shape_free(info->shape);
  }
  phy_space_remove_body(SPACE, info->body);
  phy_body_free(info->body);
}

void generate_renderer(renderer_info *info, physics_info *p_info, int tex_id, int render_layer, ecs_id linked_ent) {
  info->tex_id = tex_id;
  info->body = p_info->body;
  info->render_layer = render_layer;
  info->linked_ent = linked_ent;
  info->indicators = utl_vector_create(sizeof(indicator));
  for (int i = 0; i < RENDERER_SEGMENTS; i++) {
    info->segments[i] = NULL;
  }
}

void generate_static_renderer_segments(renderer_info *info, physics_info *p_info, phy_vect pos, int tex_id, int render_layer, ecs_id linked_ent, float angle) {
  info->tex_id = tex_id;
  info->body = p_info->body;
  info->render_layer = render_layer;
  info->linked_ent = linked_ent;
  info->indicators = utl_vector_create(sizeof(indicator));
  for (int i = 0; i < RENDERER_SEGMENTS; i++) {
    info->segments[i] = NULL;
  }
  texture_asset ta = TEXTURE_ASSET_REF[info->tex_id];
  if (ta.collision_direction == SEGMENT_BOTTOM_LEFT_RIGHT) {
    float half_width = ta.tex_width * 0.5f;
    float half_height = ta.tex_height * 0.5f;
    float radius = sqrtf(half_width * half_width + half_height * half_height);
    phy_vect top_left = phy_v(pos.x + radius * cosf(angle + atan2f(-half_height, -half_width)), pos.y + radius * sinf(angle + atan2f(-half_height, -half_width)));
    phy_vect top_right = phy_v(pos.x + radius * cosf(angle + atan2f(-half_height, half_width)), pos.y + radius * sinf(angle + atan2f(-half_height, half_width)));
    phy_vect bottom_left = phy_v(pos.x + radius * cosf(angle +atan2f(half_height, -half_width)), pos.y + radius * sinf(angle + atan2f(half_height, -half_width)));
    phy_vect bottom_right = phy_v(pos.x + radius * cosf(angle + atan2f(half_height, half_width)), pos.y + radius * sinf(angle + atan2f(half_height, half_width)));
    info->segments[0] = physics_add_static_segment_shape(SPACE, top_left, bottom_left);
    info->segments[1] = physics_add_static_segment_shape(SPACE, top_right, bottom_right);
    info->segments[2] = physics_add_static_segment_shape(SPACE, bottom_left, bottom_right);
  }
}

void free_renderer(renderer_info *info) {
  utl_vector_deallocate(info->indicators);
  for (int i = 0; i < RENDERER_SEGMENTS; i++) {
    if (info->segments[i] == NULL) {
      continue;
    }
    physics_remove_static_segment_shape(SPACE, info->segments[i]);
  }
}

void generate_destroyer(destroyer_info *info) {
  info->destroy_now = false;
}

void generate_animator(animator_info *info, int min_tex_id, int max_tex_id, float frame_duration, bool destroy_on_max) {
  info->min_tex_id = min_tex_id;
  info->max_tex_id = max_tex_id;
  info->frame_duration = frame_duration;
  info->frame_timer = frame_duration;
  info->destroy_on_max = destroy_on_max;
}

void generate_mover(mover_info *info, ecs_id entity) {
  info->body_entity = entity;
  info->target_pos_queue = utl_queue_create(sizeof(phy_vect));
}

void free_mover(mover_info *info) {
  utl_queue_deallocate(info->target_pos_queue);
}

void generate_rotator(rotator_info *info, physics_info *p_info) {
  info->body = p_info->body;
  info->target_move_pos = phy_body_get_position(info->body);
  info->target_look_pos = phy_v_add(info->target_move_pos, phy_v(0.0f, -50.0f));
}

void generate_shooter(shooter_info *info, float barrel_length) {
  info->shoot_cooldown = 0;
  info->barrel_length = barrel_length;
}

void generate_selector(selector_info *info) {
  info->selected = false;
  info->just_selected = false;
}

