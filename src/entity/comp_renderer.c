#include "comp_renderer.h"
#include "comp_physics.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include <stdlib.h>
#include <stdio.h>

ecs_id RENDERER_COMPONENT_SIGNATURE;

void comp_renderer_setup(comp_renderer *cr, comp_physics *cp) {
  cr->info.texture = (gfx_texture *)malloc(sizeof(gfx_texture));
  *cr->info.texture = gfx_load_texture_asset("Tank-Body-Blue", "png");
  cr->info.body = cp->info.body;
}

void comp_renderer_free(comp_renderer *cr) {
  free(cr->info.texture);
}

void comp_renderer_register(comp_renderer *cr, ecs_ecs *ecs) {
  cr->id = ecs_register_component(ecs, sizeof(comp_renderer), NULL, NULL);
  RENDERER_COMPONENT_SIGNATURE = cr->id; 
}

void sys_renderer_register(sys_renderer *sr, comp_renderer *cr, comp_physics *cp, ecs_ecs *ecs, renderer_info *info) {
  sr->id = ecs_register_system(ecs, sys_renderer_update, NULL, NULL, info);
  ecs_require_component(ecs, sr->id, cr->id);
  ecs_require_component(ecs, sr->id, cp->id);
  sr->ecs = *ecs;
}

ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  renderer_info *info = udata;
  for (int id = 0; id < entity_count; id++) {
    comp_renderer *cr = ecs_get(ecs, entities[id], RENDERER_COMPONENT_SIGNATURE);
    comp_physics *cp = ecs_get(ecs, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    cpVect pos = cpBodyGetPosition(info->body);
    gfx_image_no_block(pos.x, pos.y, *info->texture);
  }
  return 0;
}

