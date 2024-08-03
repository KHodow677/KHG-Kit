#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  float x, y;
} test_pos_t;

typedef struct {
  float vx, vy;
} test_vel_t;

typedef struct {
  int x, y, w, h;
} test_rect_t;

extern ecs_id_t test_pos_comp;
extern ecs_id_t test_vel_comp;
extern ecs_id_t test_rect_comp;

extern ecs_id_t test_system_1;
extern ecs_id_t test_system_2;
extern ecs_id_t test_system_3;

int ecs_test(void);

