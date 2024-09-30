#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
} animator_info;

typedef struct {
  ecs_id id;
  int min_tex_id;
  int max_tex_id;
  float frame_duration;
  float frame_timer;
  bool destroy_on_max;
} comp_animator;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_animator;

extern ecs_id ANIMATOR_COMPONENT_SIGNATURE;

void comp_animator_register(void);

void sys_animator_register(sys_animator *sa);
comp_animator *sys_animator_add(ecs_id eid);

