#include "entity/comp_spawn.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include "menus/game_menu_manager.h"
#include "menus/spawn_menu.h"
#include <stdio.h>

ecs_id SPAWNER_COMPONENT_SIGNATURE;

static ecs_ret sys_spawn_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    selector_info *s_info = utl_vector_at(SELECTOR_INFO, entities[id]);
    renderer_info *r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    if (s_info->selected) {
      SPAWN_SETTINGS.spawn_body = p_info->body;
      SPAWN_SETTINGS.spawner_tex_id = r_info->tex_id;
      GAME_OVERLAY_TRACKER[SPAWN_MENU].active = true;
    }
    else if (SPAWN_SETTINGS.spawn_body == p_info->body) {
      GAME_OVERLAY_TRACKER[SPAWN_MENU].active = false;
    }
  }
  return 0;
}

void comp_spawn_register(comp_spawn *cs) {
  cs->id = ecs_register_component(ECS, sizeof(comp_spawn), NULL, NULL);
  SPAWNER_COMPONENT_SIGNATURE = cs->id; 
}

void sys_spawn_register(sys_spawn *ss) {
  ss->id = ecs_register_system(ECS, sys_spawn_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ss->id, SPAWNER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, SELECTOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, RENDERER_COMPONENT_SIGNATURE);
  ss->ecs = *ECS;
}

void sys_spawn_add(ecs_id *eid) {
  ecs_add(ECS, *eid, SPAWNER_COMPONENT_SIGNATURE, NULL);
}

