#pragma once

#include "khg_thd/concurrent.h"
#include <stddef.h>
#include <stdbool.h>

typedef int (*thd_task_function)(void *);

typedef struct thd_task thd_task;
struct thd_task {
  thd_task_function function;
  void *arg;
  thd_task *next;
};

typedef struct thd_task_queue {
  thd_task *front;
  thd_task *rear;
  int count;
  thd_mutex lock;
  thd_thread_condition hasTasks;
} thd_task_queue;

typedef struct thd_thread_pool {
  int num_threads;
  thd_thread* threads;
  thd_task_queue queue;
  volatile bool keep_alive;
  volatile int num_working;
  thd_mutex work_mutex;
  thd_thread_condition all_idle;
} thd_thread_pool;

thd_thread_pool* thd_thread_pool_create(int num_threads);
void thd_thread_pool_add_task(thd_thread_pool* pool, thd_task_function function, void* arg);
void thd_thread_pool_wait(thd_thread_pool* pool);
void thd_thread_pool_destroy(thd_thread_pool* pool);

