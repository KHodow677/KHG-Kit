#include "khg_thd/thread_pool.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static int thread_pool_worker(void *arg) {
  thd_thread_pool *pool = (thd_thread_pool *)arg;
  while (pool->keep_alive) {
    thd_mutex_lock(&pool->queue.lock);
    while (pool->queue.count == 0 && pool->keep_alive) {
      thd_condition_wait(&pool->queue.hasTasks, &pool->queue.lock);
    }
    if (!pool->keep_alive) {
      thd_mutex_unlock(&pool->queue.lock);
      break;
    }
    thd_task* task = pool->queue.front;
    if(task != NULL) {
      pool->queue.front = task->next;
      pool->queue.count--;
      thd_mutex_unlock(&pool->queue.lock);
      if (task->function) {
          task->function(task->arg);
      }
      free(task);
    } 
    else {
      thd_mutex_unlock(&pool->queue.lock);
    }
  }
  thd_mutex_lock(&pool->work_mutex);
  pool->num_working--;
  if (pool->num_working == 0) {
    thd_condition_signal(&pool->all_idle);
  }
  thd_mutex_unlock(&pool->work_mutex);
  return 0;
}

static void task_queue_init(thd_task_queue *queue) {
  queue->front = queue->rear = NULL;
  queue->count = 0;
  thd_mutex_init(&queue->lock, THD_MUTEX_PLAIN);
  thd_condition_init(&queue->hasTasks);
}

static void task_queue_push(thd_task_queue *queue, thd_task *task) {
  thd_mutex_lock(&queue->lock);
  if (queue->rear == NULL) {
    queue->front = queue->rear = task;
  } 
  else {
    queue->rear->next = task;
    queue->rear = task;
  }
  task->next = NULL;
  queue->count++;
  thd_condition_signal(&queue->hasTasks);
  thd_mutex_unlock(&queue->lock);
}

thd_thread_pool *thd_thread_pool_create(int num_threads) {
  thd_thread_pool *pool = (thd_thread_pool *)malloc(sizeof(thd_thread_pool));
  if (!pool) {
    utl_error_func("Could not allocate memory for thread pool", utl_user_defined_data);
    return NULL;
  }
  pool->num_threads = num_threads;
  pool->keep_alive = true;
  pool->num_working = 0;
  thd_mutex_init(&pool->work_mutex, THD_MUTEX_PLAIN);
  thd_condition_init(&pool->all_idle);
  task_queue_init(&pool->queue);
  pool->threads = (thd_thread *)malloc(num_threads * sizeof(thd_thread));
  if (!pool->threads) {
    utl_error_func("Could not allocate memory for worker threads", utl_user_defined_data);
    free(pool);
    return NULL;
  }
  for (int i = 0; i < num_threads; ++i) {
    if (thd_thread_create(&pool->threads[i], thread_pool_worker, pool) != THD_THREAD_SUCCESS) {
      utl_error_func("Failed to create worker thread", utl_user_defined_data);
      thd_thread_pool_destroy(pool);
      return NULL;
    }
  }
  return pool;
}

void thd_thread_pool_add_task(thd_thread_pool *pool, thd_task_function function, void *arg) {
  thd_task *newTask = (thd_task *)malloc(sizeof(thd_task));
  if (newTask == NULL) {
    utl_error_func("Could not allocating memory for new task", utl_user_defined_data);
    return;
  }
  newTask->function = function;
  newTask->arg = arg;
  newTask->next = NULL;
  task_queue_push(&pool->queue, newTask);
}

void thd_thread_pool_wait(thd_thread_pool *pool) {
  thd_mutex_lock(&pool->work_mutex);
  while (pool->queue.count > 0 || pool->num_working) {
    thd_condition_wait(&pool->all_idle, &pool->work_mutex);
  }
  thd_mutex_unlock(&pool->work_mutex);
}

void thd_thread_pool_destroy(thd_thread_pool *pool) {
  if (pool == NULL) {
    return;
  }
  thd_mutex_lock(&pool->queue.lock);
  pool->keep_alive = false;
  thd_condition_broadcast(&pool->queue.hasTasks);
  thd_mutex_unlock(&pool->queue.lock);
  for (int i = 0; i < pool->num_threads; ++i) {
    if (thd_thread_join(pool->threads[i], NULL) != THD_THREAD_SUCCESS) {
      utl_error_func("Error joining thread", utl_user_defined_data);
    } 
  }
  free(pool->threads);
  thd_mutex_destroy(&pool->work_mutex);
  thd_condition_destroy(&pool->all_idle);
  thd_mutex_destroy(&pool->queue.lock);
  thd_condition_destroy(&pool->queue.hasTasks);
  while (pool->queue.front != NULL) {
    thd_task *task = pool->queue.front;
    pool->queue.front = task->next;
    free(task);
  }
  free(pool);
}

