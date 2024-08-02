#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  float x, y;
} pos_t;

typedef struct {
  float vx, vy;
} vel_t;

typedef struct {
  int x, y, w, h;
} rect_t;

extern ecs_id_t PosComp;
extern ecs_id_t VelComp;
extern ecs_id_t RectComp;

extern ecs_id_t System1;
extern ecs_id_t System2;
extern ecs_id_t System3;

int ecs_test(void);

