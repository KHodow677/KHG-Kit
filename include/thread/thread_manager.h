#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "khg_thd/thread.h"

extern thd_thread *WORKER_THREADS;
extern int THREAD_COUNT;

typedef struct _data {
  thd_mutex mutex;
  thd_condition ready;
  char occupied;
  char *entities;
  char *entity_count;
} thread_data;

void worker_threads_setup(void);
void worker_threads_cleanup(void);
int get_optimal_thread_count(void);
