#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  float shoot_cooldown;
  float shoot_timer;
  float barrel_length;
  bool shot;
} comp_shooter;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_shooter;

extern ecs_id SHOOTER_COMPONENT_SIGNATURE;

void comp_shooter_register(void);

void sys_shooter_register(sys_shooter *ss);
comp_shooter *sys_shooter_add(ecs_id eid);

