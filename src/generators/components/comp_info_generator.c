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
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/poly_shape.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"

void generate_physics_box(physics_info *info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog) {
  float moment = cpMomentForBox(mass, width, height);
  info->body = cpSpaceAddBody(SPACE, phy_body_new(mass, moment));
  phy_body_set_position(info->body, pos);
  phy_body_set_center_of_gravity(info->body, cog);
  phy_body_set_angle(info->body, ang);
  if (collides) {
    info->shape = cpSpaceAddShape(SPACE, cpBoxShapeNew(info->body, width, height, 0.0f));
  }
  else {
    info->shape = cpSpaceAddShape(SPACE, cpBoxShapeNew(info->body, 0.0f, 0.0f, 0.0f));
  }
  cpShapeSetFriction(info->shape, 0.0f);
  info->is_moving = false;
  info->is_turning = false;
  info->target_vel = 0.0f;
  info->target_ang_vel = 0.0f;
}

void free_physics(physics_info *info) {
  cpSpaceRemoveShape(SPACE, info->shape);
  cpSpaceRemoveBody(SPACE, info->body);
  cpShapeFree(info->shape);
  phy_body_free(info->body);
}

void generate_renderer(renderer_info *info, physics_info *p_info, int tex_id) {
  info->tex_id = tex_id;
  info->body = p_info->body;
}

void generate_follower(follower_info *info, physics_info *p_info, physics_info *target_p_info, int degree, bool follow_pos, bool follow_ang) {
  info->body = p_info->body;
  info->target_body = target_p_info->body;
  info->degree = degree;
  info->follow_pos = follow_pos;
  info->follow_ang = follow_ang;
}

void generate_destroyer(destroyer_info *info) {
  info->destroy_now = false;
}

void generate_animator(animator_info *info, int min_tex_id, int max_tex_id, int frame_duration, bool destroy_on_max) {
  info->min_tex_id = min_tex_id;
  info->max_tex_id = max_tex_id;
  info->frame_duration = frame_duration;
  info->frame_timer = frame_duration;
  info->destroy_on_max = destroy_on_max;
}

void generate_mover(mover_info *info, physics_info *p_info) {
  info->body = p_info->body;
  info->target_move_pos = phy_body_get_position(info->body);
  info->target_look_pos = cpvadd(info->target_move_pos, cpv(0.0f, -50.0f));
}

void generate_rotator(rotator_info *info, physics_info *p_info) {
  info->body = p_info->body;
  info->target_move_pos = phy_body_get_position(info->body);
  info->target_look_pos = cpvadd(info->target_move_pos, cpv(0.0f, -50.0f));
}

void generate_shooter(shooter_info *info) {
  info->shoot_cooldown = 0;
}

