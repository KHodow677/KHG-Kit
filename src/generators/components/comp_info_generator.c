#include "generators/components/comp_info_generator.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"

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
  info->space = sp;
}

void free_physics(physics_info *info) {
  cpShapeFree(info->shape);
  cpBodyFree(info->body);
}

void generate_renderer(renderer_info *info, physics_info *p_info, char *file_name, char *file_type) {
  info->texture = (gfx_texture *)malloc(sizeof(gfx_texture));
  *info->texture = gfx_load_texture_asset(file_name, file_type);
  info->body = p_info->body;
}

void free_renderer(renderer_info *info) {
  free(info->texture);
}

void generate_follower(follower_info *info, physics_info *p_info, physics_info *target_p_info) {
  info->body = p_info->body;
  info->target_body = target_p_info->body;
}

