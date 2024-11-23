#include "khg_thd/thread_pool.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static int thread_pool_worker(void *arg) {
  ThreadPool *pool = (ThreadPool *)arg;
  while (pool->keepAlive) {
    mutex_lock(&pool->queue.lock);
    while (pool->queue.count == 0 && pool->keepAlive) {
      condition_wait(&pool->queue.hasTasks, &pool->queue.lock);
    }
    if (!pool->keepAlive) {
      mutex_unlock(&pool->queue.lock);
      break;
    }
    Task* task = pool->queue.front;
    if(task != NULL) {
      pool->queue.front = task->next;
      pool->queue.count--;
      mutex_unlock(&pool->queue.lock);
      if (task->function) {
          task->function(task->arg);
      }
      free(task);
    } 
    else {
      mutex_unlock(&pool->queue.lock);
    }
  }
  mutex_lock(&pool->workMutex);
  pool->numWorking--;
  if (pool->numWorking == 0) {
    condition_signal(&pool->allIdle);
  }
  mutex_unlock(&pool->workMutex);
  return 0;
}

static void task_queue_init(TaskQueue *queue) {
  queue->front = queue->rear = NULL;
  queue->count = 0;
  mutex_init(&queue->lock, MUTEX_PLAIN);
  condition_init(&queue->hasTasks);
}

static void task_queue_push(TaskQueue *queue, Task *task) {
  mutex_lock(&queue->lock);
  if (queue->rear == NULL) {
    queue->front = queue->rear = task;
  } 
  else {
    queue->rear->next = task;
    queue->rear = task;
  }
  task->next = NULL;
  queue->count++;
  condition_signal(&queue->hasTasks);
  mutex_unlock(&queue->lock);
}

ThreadPool *thread_pool_create(int num_threads) {
  ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
  if (!pool) {
    utl_error_func("Could not allocate memory for thread pool", utl_user_defined_data);
    return NULL;
  }
  pool->numThreads = num_threads;
  pool->keepAlive = true;
  pool->numWorking = 0;
  mutex_init(&pool->workMutex, MUTEX_PLAIN);
  condition_init(&pool->allIdle);
  task_queue_init(&pool->queue);
  pool->threads = (Thread *)malloc(num_threads * sizeof(Thread));
  if (!pool->threads) {
    utl_error_func("Could not allocate memory for worker threads", utl_user_defined_data);
    free(pool);
    return NULL;
  }
  for (int i = 0; i < num_threads; ++i) {
    if (thread_create(&pool->threads[i], thread_pool_worker, pool) != THREAD_SUCCESS) {
      utl_error_func("Failed to create worker thread", utl_user_defined_data);
      thread_pool_destroy(pool);
      return NULL;
    }
  }
  return pool;
}

void thread_pool_add_task(ThreadPool *pool, TaskFunction function, void *arg) {
  Task *newTask = (Task *)malloc(sizeof(Task));
  if (newTask == NULL) {
    utl_error_func("Could not allocating memory for new task", utl_user_defined_data);
    return;
  }
  newTask->function = function;
  newTask->arg = arg;
  newTask->next = NULL;
  task_queue_push(&pool->queue, newTask);
}

void thread_pool_wait(ThreadPool *pool) {
  mutex_lock(&pool->workMutex);
  while (pool->queue.count > 0 || pool->numWorking) {
    condition_wait(&pool->allIdle, &pool->workMutex);
  }
  mutex_unlock(&pool->workMutex);
}

void thread_pool_destroy(ThreadPool *pool) {
  if (pool == NULL) {
    return;
  }
  mutex_lock(&pool->queue.lock);
  pool->keepAlive = false;
  condition_broadcast(&pool->queue.hasTasks);
  mutex_unlock(&pool->queue.lock);
  for (int i = 0; i < pool->numThreads; ++i) {
    if (thread_join(pool->threads[i], NULL) != THREAD_SUCCESS) {
      utl_error_func("Error joining thread", utl_user_defined_data);
    } 
  }
  free(pool->threads);
  mutex_destroy(&pool->workMutex);
  condition_destroy(&pool->allIdle);
  mutex_destroy(&pool->queue.lock);
  condition_destroy(&pool->queue.hasTasks);
  while (pool->queue.front != NULL) {
    Task *task = pool->queue.front;
    pool->queue.front = task->next;
    free(task);
  }
  free(pool);
}

