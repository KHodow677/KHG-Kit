#define NAMESPACE_TASKING_IMPL

#define MAX_TASKS 100
#define THREAD_POOL_SIZE 5

#include "khg_thd/concurrent.h"
#include "tasking/namespace.h"
#include <stdbool.h>

static worker_task TASK_QUEUE[MAX_TASKS];
static int QUEUE_START = 0;
static int QUEUE_END = 0;
static thd_mutex QUEUE_MUTEX;
static thd_thread_condition QUEUE_NOT_EMPTY;
static thd_thread_condition QUEUE_NOT_FULL;
static bool THREAD_POOL_SHUTDOWN = false;

void task_enqueue(void (*function)(void *), void *arg) {
  thd_mutex_lock(&QUEUE_MUTEX);
  while ((QUEUE_END + 1) % MAX_TASKS == QUEUE_START) {
    thd_condition_wait(&QUEUE_NOT_FULL, &QUEUE_MUTEX);
  }
  TASK_QUEUE[QUEUE_END].function = function;
  TASK_QUEUE[QUEUE_END].arg = arg;
  QUEUE_END = (QUEUE_END + 1) % MAX_TASKS;
  thd_condition_signal(&QUEUE_NOT_EMPTY);
  thd_mutex_unlock(&QUEUE_MUTEX);
}

worker_task task_dequeue() {
  worker_task task;
  thd_mutex_lock(&QUEUE_MUTEX);
  while (QUEUE_START == QUEUE_END && !THREAD_POOL_SHUTDOWN) {
    thd_condition_wait(&QUEUE_NOT_EMPTY, &QUEUE_MUTEX);
  }
  if (THREAD_POOL_SHUTDOWN) {
    thd_mutex_unlock(&QUEUE_MUTEX);
    thd_thread_exit(0);
  }
  task = TASK_QUEUE[QUEUE_START];
  QUEUE_START = (QUEUE_START + 1) % MAX_TASKS;
  thd_condition_signal(&QUEUE_NOT_FULL);
  thd_mutex_unlock(&QUEUE_MUTEX);
  return task;
}

int task_worker(void *arg) {
  (void)arg;
  while (1) {
    worker_task task = task_dequeue();
    if (task.function) {
      task.function(task.arg);
    }
  }
  return 0; 
}

void initialize_thread_pool() {
  thd_mutex_init(&QUEUE_MUTEX, THD_MUTEX_PLAIN);
  thd_condition_init(&QUEUE_NOT_EMPTY);
  thd_condition_init(&QUEUE_NOT_FULL);
  thd_thread threads[THREAD_POOL_SIZE];
  for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
    thd_thread_create(&threads[i], task_worker, NULL);
  }
}

void shutdown_thread_pool() {
  thd_mutex_lock(&QUEUE_MUTEX);
  THREAD_POOL_SHUTDOWN = true;
  thd_condition_broadcast(&QUEUE_NOT_EMPTY);
  thd_mutex_unlock(&QUEUE_MUTEX);
}
