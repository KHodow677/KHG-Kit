#define NAMESPACE_TASKING_IMPL

#define MAX_TASKS 100
#define THREAD_POOL_SIZE 5

#include "khg_thd/concurrent.h"
#include "khg_utl/queue.h"
#include "tasking/namespace.h"
#include <stdbool.h>

static utl_queue *TASK_QUEUE;
static thd_mutex QUEUE_MUTEX;
static thd_thread_condition QUEUE_NOT_EMPTY;
static thd_thread_condition QUEUE_NOT_FULL;
static bool THREAD_POOL_SHUTDOWN = false;
static thd_thread THREADS[THREAD_POOL_SIZE];

void task_enqueue(void (*function)(void *), void *arg) {
  thd_mutex_lock(&QUEUE_MUTEX);
  while (utl_queue_size(TASK_QUEUE) >= MAX_TASKS) {
    thd_condition_wait(&QUEUE_NOT_FULL, &QUEUE_MUTEX);
  }
  worker_task task = { function, arg };
  utl_queue_push(TASK_QUEUE, &task);
  thd_condition_signal(&QUEUE_NOT_EMPTY);
  thd_mutex_unlock(&QUEUE_MUTEX);
}

worker_task task_dequeue() {
  worker_task task;
  thd_mutex_lock(&QUEUE_MUTEX);
  while (utl_queue_empty(TASK_QUEUE) && !THREAD_POOL_SHUTDOWN) {
    thd_condition_wait(&QUEUE_NOT_EMPTY, &QUEUE_MUTEX);
  }
  if (THREAD_POOL_SHUTDOWN) {
    thd_mutex_unlock(&QUEUE_MUTEX);
    thd_thread_exit(0);
  }
  task = *(worker_task *)utl_queue_front(TASK_QUEUE);
  utl_queue_pop(TASK_QUEUE);
  thd_condition_signal(&QUEUE_NOT_FULL);
  thd_mutex_unlock(&QUEUE_MUTEX);
  return task;
}

int task_worker(void *arg) {
  while (true) {
    worker_task task = task_dequeue();
    if (task.function) {
      task.function(task.arg);
    }
  }
  return 0; 
}

void initialize_thread_pool() {
  TASK_QUEUE = utl_queue_create(sizeof(worker_task));
  thd_mutex_init(&QUEUE_MUTEX, THD_MUTEX_PLAIN);
  thd_condition_init(&QUEUE_NOT_EMPTY);
  thd_condition_init(&QUEUE_NOT_FULL);
  for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
    thd_thread_create(&THREADS[i], task_worker, NULL);
  }
}

void shutdown_thread_pool() {
  thd_mutex_lock(&QUEUE_MUTEX);
  THREAD_POOL_SHUTDOWN = true;
  thd_condition_broadcast(&QUEUE_NOT_EMPTY);
  thd_mutex_unlock(&QUEUE_MUTEX);
  for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
    thd_thread_join(THREADS[i], NULL);
  }
  utl_queue_deallocate(TASK_QUEUE);
}

