#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  float bar_width;
  float bar_height;
  int number_of_bars;
  bool tracks_health;
  bool tracks_ammo;
  bool tracks_fuel;
} comp_status;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_status;

extern ecs_id STATUS_COMPONENT_SIGNATURE;

void comp_status_register(void);

void sys_status_register(sys_status *ss);
comp_status *sys_status_add(ecs_id eid);

