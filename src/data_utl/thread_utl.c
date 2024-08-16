#include "data_utl/thread_utl.h"
#include <stdio.h>

int THREAD_COUNT;

void set_optimal_thread_count() {
#if defined(_WIN32) || defined(_WIN64)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  THREAD_COUNT = (int)sysinfo.dwNumberOfProcessors;
  printf("Number of Threads: %i\n", THREAD_COUNT);
#else
  long cores = sysconf(_SC_NPROCESSORS_ONLN);
  THREAD_COUNT = (int)(cores > 0 ? cores : 1);
  printf("Number of Threads: %i\n", THREAD_COUNT);
#endif
}

