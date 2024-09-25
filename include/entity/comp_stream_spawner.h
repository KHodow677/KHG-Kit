#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/queue.h"

typedef struct {
  float spawn_cooldown;
  float spawn_count;
  utl_queue *spawn_queue;
} stream_spawner_info;

typedef struct {
  ecs_id id;
} comp_stream_spawner;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_stream_spawner;

extern ecs_id STREAM_SPAWNER_COMPONENT_SIGNATURE;
extern stream_spawner_info NO_STREAM_SPAWNER;
extern utl_vector *STREAM_SPAWNER_INFO;

void comp_stream_spawner_register(comp_stream_spawner *css);

void sys_stream_spawner_register(sys_stream_spawner *sss);
void sys_stream_spawner_add(ecs_id *eid, stream_spawner_info *info);

