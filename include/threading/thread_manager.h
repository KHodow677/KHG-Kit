#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "khg_thd/thread.h"

typedef struct _data {
  thd_mutex mutex;
  thd_condition condwrite;
  thd_condition condread;
  char occupied;
  char buffer[256];
} thread_data;

void setup_worker_threads(void);
void free_worker_threads(void);
int get_optimal_thread_count(void);
