#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  float x, y;
} pos_t;

typedef struct {
  float vx, vy;
} vel_t;

extern ecs_id_t poscomp;
extern ecs_id_t velcomp;
extern ecs_id_t rectcomp;

extern ecs_id_t system1;
extern ecs_id_t system2;
extern ecs_id_t system3;

int ecs_setup(void);

