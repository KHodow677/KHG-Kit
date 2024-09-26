#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  int min_tex_id;
  int max_tex_id;
  float frame_duration;
  float frame_timer;
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
extern animator_info *ANIMATOR_INFO;

void comp_animator_register(comp_animator *ca);

void sys_animator_register(sys_animator *sa);
void sys_animator_add(ecs_id *eid, animator_info *info);

