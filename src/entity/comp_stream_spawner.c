#include "entity/comp_stream_spawner.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"
#include "spawners/spawn_slug.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id STREAM_SPAWNER_COMPONENT_SIGNATURE;
stream_spawner_info NO_STREAM_SPAWNER = { 0 };
utl_vector *STREAM_SPAWNER_INFO = NULL;

static ecs_ret sys_stream_spawner_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    stream_spawner_info *info = utl_vector_at(STREAM_SPAWNER_INFO, entities[id]);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    if (utl_queue_size(info->spawn_queue) > 0 && info->spawn_timer <= 0) {
      int *next_spawn_type = utl_queue_front(info->spawn_queue);
      if (*next_spawn_type == SPAWN_SLUG) {
        phy_vect pos = phy_body_get_position(p_info->body);
        float ang = normalize_angle(phy_body_get_angle(p_info->body));
        spawn_slug(pos.x + info->spawn_offset.x, pos.y + info->spawn_offset.y, ang, (phy_vect *)utl_vector_data(info->path), utl_vector_size(info->path));
      }
      info->spawn_timer = info->spawn_cooldown;
      utl_queue_pop(info->spawn_queue);
    }
    info->spawn_timer -= dt;
  }
  return 0;
}

void comp_stream_spawner_register(comp_stream_spawner *css) {
  css->id = ecs_register_component(ECS, sizeof(comp_stream_spawner), NULL, NULL);
  STREAM_SPAWNER_COMPONENT_SIGNATURE = css->id; 
}

void sys_stream_spawner_register(sys_stream_spawner *sss) {
  sss->id = ecs_register_system(ECS, sys_stream_spawner_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sss->id, STREAM_SPAWNER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sss->id, PHYSICS_COMPONENT_SIGNATURE);
  sss->ecs = *ECS;
  STREAM_SPAWNER_INFO = utl_vector_create(sizeof(stream_spawner_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(STREAM_SPAWNER_INFO, &NO_STREAM_SPAWNER);
  }
}

void sys_stream_spawner_add(ecs_id *eid, stream_spawner_info *info) {
  ecs_add(ECS, *eid, STREAM_SPAWNER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(STREAM_SPAWNER_INFO, *eid, info);
}

