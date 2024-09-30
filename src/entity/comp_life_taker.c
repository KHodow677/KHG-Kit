#include "entity/comp_life_taker.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_mover.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include "menus/pause_menu.h"
#include <stdio.h>

ecs_id LIFE_TAKER_COMPONENT_SIGNATURE;

static ecs_ret sys_life_taker_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_life_taker *info = ecs_get(ECS, entities[id], LIFE_TAKER_COMPONENT_SIGNATURE);
    comp_mover *m_info = ecs_get(ECS, entities[id], MOVER_COMPONENT_SIGNATURE);
    comp_destroyer *d_info = ecs_get(ECS, entities[id], DESTROYER_COMPONENT_SIGNATURE);
    if (utl_vector_size(m_info->target_pos_queue->vec) == 0) {
      GAME_INFO.lives -= info->num_lives;
      d_info->destroy_now = true;
      if (GAME_INFO.lives <= 0) {
        PAUSED = true;
        handle_pause();
      }
    }
  }
  return 0;
}

void comp_life_taker_register() {
  LIFE_TAKER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_life_taker), NULL, NULL);
}

void sys_life_taker_register(sys_life_taker *slt) {
  slt->id = ecs_register_system(ECS, sys_life_taker_update, NULL, NULL, NULL);
  ecs_require_component(ECS, slt->id, LIFE_TAKER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, slt->id, MOVER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, slt->id, DESTROYER_COMPONENT_SIGNATURE);
  slt->ecs = *ECS;
}

comp_life_taker *sys_life_taker_add(ecs_id eid) {
  return ecs_add(ECS, eid, LIFE_TAKER_COMPONENT_SIGNATURE, NULL);
}

