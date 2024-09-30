#include "entity/comp_spawn.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "menus/game_menu_manager.h"
#include "menus/spawn_menu.h"
#include <stdio.h>

ecs_id SPAWNER_COMPONENT_SIGNATURE;

static ecs_ret sys_spawn_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_spawn *info = ecs_get(ECS, entities[id], SPAWNER_COMPONENT_SIGNATURE);
    comp_selector *s_info = ecs_get(ECS, entities[id], SELECTOR_COMPONENT_SIGNATURE);
    comp_renderer *r_info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    if (s_info->selected) {
      SPAWN_SETTINGS.pos = info->pos;
      SPAWN_SETTINGS.linked_pos = info->linked_pos;
      SPAWN_SETTINGS.ang = phy_body_get_angle(p_info->body);
      SPAWN_SETTINGS.comp_selector = s_info;
      SPAWN_SETTINGS.spawner_tex_id = r_info->tex_id;
      GAME_OVERLAY_TRACKER[SPAWN_MENU].active = true;
    }
    else if (SPAWN_SETTINGS.comp_selector == s_info) {
      GAME_OVERLAY_TRACKER[SPAWN_MENU].active = false;
    }
  }
  return 0;
}

void comp_spawn_register() {
  SPAWNER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_spawn), NULL, NULL);
}

void sys_spawn_register(sys_spawn *ss) {
  ss->id = ecs_register_system(ECS, sys_spawn_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ss->id, SPAWNER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, SELECTOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, RENDERER_COMPONENT_SIGNATURE);
  ss->ecs = *ECS;
}

comp_spawn *sys_spawn_add(ecs_id eid) {
  return ecs_add(ECS, eid, SPAWNER_COMPONENT_SIGNATURE, NULL);
}

