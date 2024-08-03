#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  float x, y;
} pos_t;

extern ecs_id poscomp;

extern ecs_id system1;

int ecs_setup(void);

