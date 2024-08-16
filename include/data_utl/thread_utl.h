#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "khg_ecs/ecs.h"

extern int THREAD_COUNT;

typedef struct {
  ecs_id *entities;
  int start;
  int end;
} thread_data;

void set_optimal_thread_count(void);

