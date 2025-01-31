#define NAMESPACE_TASKING_IMPL

#define MAX_TASKS 100
#define NUM_THREADS 5

#include "khg_thd/concurrent.h"
#include "tasking/namespace.h"
#include <stdbool.h>

tasking_task task_queue[MAX_TASKS];
int task_count = 0;
thd_mutex queue_mutex;
thd_thread_condition has_tasks;

static void task_enqueue(void (*function)(void *), void *arg) {
  thd_mutex_lock(&queue_mutex);
  if (task_count < MAX_TASKS) {
    tasking_task new_task;
    new_task.function = function;
    new_task.arg = arg;
    task_queue[task_count++] = new_task;
    thd_condition_signal(&has_tasks);
  }
  thd_mutex_unlock(&queue_mutex);
}

static tasking_task *task_dequeue() {
  tasking_task *task = NULL;
  if (task_count > 0) {
    task = &task_queue[--task_count];
  }
  return task;
}

static int task_worker(void *arg) {
  (void)arg;
  while (true) {
    thd_mutex_lock(&queue_mutex);
    while (task_count == 0) {
      thd_condition_wait(&has_tasks, &queue_mutex);
    }
    tasking_task* task = task_dequeue();
    thd_mutex_unlock(&queue_mutex);
    if (task) {
      task->function(task->arg);
    }
  }
  return -1;
}

