#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"

typedef struct {
  int min_tex_id;
  int max_tex_id;
  int frame_duration;
  int frame_timer;
  bool destroy_on_max;
} animator_info;

typedef struct {
  ecs_id id;
} comp_animator;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_animator;

extern ecs_id ANIMATOR_COMPONENT_SIGNATURE;
extern animator_info NO_ANIMATOR;
extern utl_vector *ANIMATOR_INFO;

void comp_animator_register(comp_animator *ca, ecs_ecs *ecs);

void sys_animator_register(sys_animator *sa, ecs_ecs *ecs);
void sys_animator_add(ecs_ecs *ecs, ecs_id *eid, animator_info *info);
void sys_animator_free(bool need_free);
void *update_animator_entities(void *arg);
ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

