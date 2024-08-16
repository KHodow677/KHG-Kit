#include "data_utl/thread_utl.h"
#include "khg_thd/thread.h"
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

ecs_ret run_thread_update(ecs_id *entities, int entity_count, void *(*func_ptr)(void *)) {
  if (entity_count == 0) {
    return 0;
  }
  const int thread_count = THREAD_COUNT;
  struct thd_thread threads[thread_count];
  thread_data t_data[thread_count];
  int chunk_size = entity_count / thread_count;
  for (int i = 0; i < thread_count; i++) {
    t_data[i].entities = entities;
    t_data[i].start = i * chunk_size;
    t_data[i].end = (i == thread_count - 1) ? entity_count : t_data[i].start + chunk_size;
    thd_thread_create(&threads[i], NULL, func_ptr, &t_data[i], NULL);
  }
  for (int i = 0; i < thread_count; i++) {
    thd_thread_join(threads[i], NULL);
  }
  return 0;
}

