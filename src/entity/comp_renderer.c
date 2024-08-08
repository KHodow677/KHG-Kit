#include "comp_renderer.h"
#include "comp_physics.h"
#include "../data_utl/map_utl.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include <stdlib.h>
#include <stdio.h>

ecs_id RENDERER_COMPONENT_SIGNATURE;
map *RENDERER_INFO_MAP = NULL;

void info_renderer_setup(renderer_info *info, physics_info *p_info, char *file_name, char *file_type) {
  info->texture = (gfx_texture *)malloc(sizeof(gfx_texture));
  *info->texture = gfx_load_texture_asset(file_name, file_type);
  info->body = p_info->body;
}

void info_renderer_free(renderer_info *info) {
  free(info->texture);
}

void comp_renderer_register(comp_renderer *cr, ecs_ecs *ecs) {
  cr->id = ecs_register_component(ecs, sizeof(comp_renderer), NULL, NULL);
  RENDERER_COMPONENT_SIGNATURE = cr->id; 
}

void sys_renderer_register(sys_renderer *sr, ecs_ecs *ecs) {
  sr->id = ecs_register_system(ecs, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sr->id, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sr->id, RENDERER_COMPONENT_SIGNATURE);
  sr->ecs = *ecs;
  RENDERER_INFO_MAP = map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_renderer_add(ecs_ecs *ecs, ecs_id *eid, renderer_info *info) {
  ecs_add(ecs, *eid, RENDERER_COMPONENT_SIGNATURE, NULL);
  map_insert(RENDERER_INFO_MAP, eid, info);
}

void sys_renderer_free(bool need_free) {
  if (need_free) {
    map_deallocate(RENDERER_INFO_MAP);
  }
}

ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  renderer_info *info = map_at(RENDERER_INFO_MAP, &entities[0]);
  for (int id = 0; id < entity_count; id++) {
    info = map_at(RENDERER_INFO_MAP, &entities[id]);
    cpVect pos = cpBodyGetPosition(info->body);
    cpFloat angle = cpBodyGetAngle(info->body);
    info->texture->angle = angle;
    gfx_image_no_block(pos.x, pos.y, *info->texture);
  }
  return 0;
}

