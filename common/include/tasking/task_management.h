#include "tasking/namespace.h"
#include <stdbool.h>

void task_enqueue(void (*function)(void *), void *arg);
worker_task task_dequeue(void);
int task_worker(void *arg);

void initialize_thread_pool(void);
void shutdown_thread_pool(void);

