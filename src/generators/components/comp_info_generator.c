#include "generators/components/comp_info_generator.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "game_manager.h"
#include "khg_phy/body.h"
#include "khg_phy/constraint.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/pivot_joint.h"
#include "khg_phy/poly_shape.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"
#include <stdio.h>

void generate_physics_box(physics_info *info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog) {
  float moment = phy_moment_for_box(mass, width, height);
  info->body = phy_space_add_body(SPACE, phy_body_new(mass, moment));
  phy_body_set_position(info->body, pos);
  phy_body_set_center_of_gravity(info->body, cog);
  phy_body_set_angle(info->body, ang);
  if (collides) {
    info->shape = phy_space_add_shape(SPACE, phy_box_shape_new(info->body, width, height, 0.0f));
  }
  else {
    info->shape = phy_space_add_shape(SPACE, phy_box_shape_new(info->body, 0.0f, 0.0f, 0.0f));
  }
  phy_shape_set_friction(info->shape, 0.0f);
  info->is_moving = false;
  info->is_turning = false;
  info->target_vel = 0.0f;
  info->target_ang_vel = 0.0f;
}

void free_physics(physics_info *info) {
  phy_space_remove_shape(SPACE, info->shape);
  phy_space_remove_body(SPACE, info->body);
  phy_shape_free(info->shape);
  phy_body_free(info->body);
}

void generate_renderer(renderer_info *info, phy_body *body, int tex_id) {
  info->tex_id = tex_id;
  info->body = body;
}

void generate_follower(follower_info *info, physics_info *target_p_info, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog) {
  float moment = phy_moment_for_box(mass, width, height);
  info->body = phy_space_add_body(SPACE, phy_body_new(mass, moment));
  phy_body_set_position(info->body, pos);
  phy_body_set_center_of_gravity(info->body, cog);
  phy_body_set_angle(info->body, ang);
  info->target_body = target_p_info->body;
  info->pivot = phy_space_add_constraint(SPACE, phy_pivot_joint_new_2(info->target_body, info->body, phy_v_zero, cog));
}

void free_follower(follower_info *info) {
  phy_space_remove_constraint(SPACE, info->pivot);
  phy_space_remove_body(SPACE, info->body);
  phy_constraint_free(info->pivot);
  phy_body_free(info->body);
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

void generate_mover(mover_info *info, physics_info *p_info) {
  info->body = p_info->body;
  info->target_move_pos = phy_body_get_position(info->body);
  info->target_look_pos = phy_v_add(info->target_move_pos, phy_v(0.0f, -50.0f));
}

void generate_rotator(rotator_info *info, physics_info *p_info) {
  info->body = p_info->body;
  info->target_move_pos = phy_body_get_position(info->body);
  info->target_look_pos = phy_v_add(info->target_move_pos, phy_v(0.0f, -50.0f));
}

void generate_shooter(shooter_info *info) {
  info->shoot_cooldown = 0;
}
