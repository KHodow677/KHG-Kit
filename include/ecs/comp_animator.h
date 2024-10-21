#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  int min_tex_id;
  int max_tex_id;
  float frame_duration;
  float frame_timer;
  bool destroy_on_max;
} comp_animator;

typedef struct {
  int min_tex_id;
  int max_tex_id;
  float frame_duration;
  bool destroy_on_max;
} comp_animator_constructor_info;

extern ecs_id ANIMATOR_COMPONENT_SIGNATURE;
extern ecs_id ANIMATOR_SYSTEM_SIGNATURE;

extern comp_animator_constructor_info *ANIMATOR_CONSTRUCTOR_INFO;

void comp_animator_register(void);
void sys_animator_register(void);

comp_animator *sys_animator_add(ecs_id eid, comp_animator_constructor_info *cpci);

