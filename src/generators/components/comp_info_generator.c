#include "generators/components/comp_info_generator.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_gfx/texture.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

void generate_physics_box(physics_info *info, cpSpace *sp, bool collides, float width, float height, float mass, cpVect pos, cpFloat ang, cpVect cog) {
  float moment = cpMomentForBox(mass, width, height);
  info->body = cpSpaceAddBody(sp, cpBodyNew(mass, moment));
  cpBodySetPosition(info->body, pos);
  cpBodySetCenterOfGravity(info->body, cog);
  cpBodySetAngle(info->body, ang);
  if (collides) {
    info->shape = cpSpaceAddShape(sp, cpBoxShapeNew(info->body, width, height, 0.0f));
  }
  else {
    info->shape = cpSpaceAddShape(sp, cpBoxShapeNew(info->body, 0.0f, 0.0f, 0.0f));
  }
  cpShapeSetFriction(info->shape, 0.0f);
  info->is_moving = false;
  info->is_turning = false;
  info->target_vel = 0.0f;
  info->target_ang_vel = 0.0f;
}

void free_physics(physics_info *info, cpSpace *sp) {
  cpSpaceRemoveShape(sp, info->shape);
  cpSpaceRemoveBody(sp, info->body);
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

void generate_animator(animator_info *info, utl_vector *vec, int frame_count) {
  info->tex_vec = vec;
  info->frame_count = frame_count;
  info->frames_left = frame_count;
}

void free_animator(animator_info *info) {
  for (size_t i = 0; i < utl_vector_size(info->tex_vec); i++) {
    gfx_texture **tex = utl_vector_at(info->tex_vec, i);
    free(*tex);
  }
  utl_vector_deallocate(info->tex_vec);
}

