#include "generators/components/comp_info_generator.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"

void generate_physics_box(physics_info *info, bool collides, float width, float height, float mass, cpVect pos, cpFloat ang, cpVect cog) {
  float moment = cpMomentForBox(mass, width, height);
  info->body = cpSpaceAddBody(SPACE, cpBodyNew(mass, moment));
  cpBodySetPosition(info->body, pos);
  cpBodySetCenterOfGravity(info->body, cog);
  cpBodySetAngle(info->body, ang);
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
  cpBodyFree(info->body);
}

void generate_renderer(renderer_info *info, physics_info *p_info, int tex_id) {
  info->tex_id = tex_id;
  info->body = p_info->body;
}

void generate_follower(follower_info *info, physics_info *p_info, physics_info *target_p_info, bool follow_pos, bool follow_ang) {
  info->body = p_info->body;
  info->target_body = target_p_info->body;
  info->follow_pos = follow_pos;
  info->follow_ang = follow_ang;
}

void generate_destroyer(destroyer_info *info) {
  info->destroy_now = false;
}

void generate_animator(animator_info *info, int min_tex_id, int max_tex_id, int frame_duration) {
  info->min_tex_id = min_tex_id;
  info->max_tex_id = max_tex_id;
  info->frame_duration = frame_duration;
  info->frame_timer = frame_duration;
}

