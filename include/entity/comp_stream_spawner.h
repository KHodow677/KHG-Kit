#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/queue.h"

typedef enum {
  SPAWN_SLUG
} spawn_type;

typedef struct {
  ecs_id id;
  bool spawn_infinitely;
  float spawn_cooldown;
  float update_cooldown;
  float update_timer;
  float spawn_timer;
  phy_vect spawn_offset;
  float spawn_health;
  utl_queue *spawn_queue;
  utl_vector *path;
} comp_stream_spawner;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_stream_spawner;

extern ecs_id STREAM_SPAWNER_COMPONENT_SIGNATURE;

void comp_stream_spawner_register(void);

void sys_stream_spawner_register(sys_stream_spawner *sss);
comp_stream_spawner *sys_stream_spawner_add(ecs_id eid);

