#include "thread/thread_manager.h"
#include <stdio.h>

thd_thread NETWORKING_THREAD;
int THREAD_COUNT = 0;

void worker_threads_setup() {
  THREAD_COUNT = get_optimal_thread_count();
}

void worker_threads_cleanup() {
}

const int get_optimal_thread_count() {
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

