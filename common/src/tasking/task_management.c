#define NAMESPACE_TASKING_IMPL

#define MAX_TASKS 100
#define THREAD_POOL_SIZE 5

#include "khg_thd/concurrent.h"
#include "tasking/namespace.h"
#include <stdbool.h>

worker_task task_queue[MAX_TASKS];
int queue_start = 0;
int queue_end = 0;
thd_mutex queue_mutex;
thd_thread_condition queue_not_empty;
thd_thread_condition queue_not_full;
bool thread_pool_shutdown = false;

void task_enqueue(void (*function)(void *), void *arg) {
  thd_mutex_lock(&queue_mutex);
  while ((queue_end + 1) % MAX_TASKS == queue_start) {
    thd_condition_wait(&queue_not_full, &queue_mutex);
  }
  task_queue[queue_end].function = function;
  task_queue[queue_end].arg = arg;
  queue_end = (queue_end + 1) % MAX_TASKS;
  thd_condition_signal(&queue_not_empty);
  thd_mutex_unlock(&queue_mutex);
}

worker_task task_dequeue() {
  worker_task task;
  thd_mutex_lock(&queue_mutex);
  while (queue_start == queue_end && !thread_pool_shutdown) {
    thd_condition_wait(&queue_not_empty, &queue_mutex);
  }
  if (thread_pool_shutdown) {
    thd_mutex_unlock(&queue_mutex);
    thd_thread_exit(0);
  }
  task = task_queue[queue_start];
  queue_start = (queue_start + 1) % MAX_TASKS;
  thd_condition_signal(&queue_not_full);
  thd_mutex_unlock(&queue_mutex);
  return task;
}

static int task_worker(void *arg) {
  (void)arg;
  while (1) {
    worker_task task = task_dequeue();
    if (task.function) {
      task.function(task.arg);
    }
  }
  return 0; 
}

static void initialize_thread_pool() {
  thd_mutex_init(&queue_mutex, THD_MUTEX_PLAIN);
  thd_condition_init(&queue_not_empty);
  thd_condition_init(&queue_not_full);
  thd_thread threads[THREAD_POOL_SIZE];
  for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
    thd_thread_create(&threads[i], task_worker, NULL);
  }
}

static void shutdown_thread_pool() {
  thd_mutex_lock(&queue_mutex);
  thread_pool_shutdown = true;
  thd_condition_broadcast(&queue_not_empty);
  thd_mutex_unlock(&queue_mutex);
}

