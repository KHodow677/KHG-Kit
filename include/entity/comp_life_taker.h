#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  int num_lives;
} comp_life_taker;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_life_taker;

extern ecs_id LIFE_TAKER_COMPONENT_SIGNATURE;

void comp_life_taker_register(void);

void sys_life_taker_register(sys_life_taker *slt);
comp_life_taker *sys_life_taker_add(ecs_id eid);

