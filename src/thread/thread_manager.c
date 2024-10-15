#include "thread/thread_manager.h"
#include "khg_thd/thread.h"
#include <stdio.h>
#include <stdlib.h>

thd_thread *WORKER_THREADS = NULL;
int THREAD_COUNT = 0;

void worker_threads_setup() {
  THREAD_COUNT = get_optimal_thread_count();
  WORKER_THREADS = (thd_thread *)malloc(THREAD_COUNT * sizeof(thd_thread));
}

void worker_threads_cleanup() {
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
