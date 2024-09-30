#include "threading/thread_manager.h"
#include "game_manager.h"
#include "khg_thd/thread.h"
#include <stdio.h>
#include <stdlib.h>

void setup_worker_threads(void) {
  THREAD_COUNT = get_optimal_thread_count();
  WORKER_THREADS = (thd_thread *)malloc(THREAD_COUNT * sizeof(thd_thread));
}

void free_worker_threads(void) {
  free(WORKER_THREADS);
}

int get_optimal_thread_count() {
#if defined(_WIN32) || defined(_WIN64)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  int ret = sysinfo.dwNumberOfProcessors;
  printf("Number of Threads: %i\n", ret);
  return ret;
#else
  long cores = sysconf(_SC_NPROCESSORS_ONLN);
  int ret = (cores > 0 ? cores : 1);
  printf("Number of Threads: %i\n", ret);
  return ret;
#endif
}

