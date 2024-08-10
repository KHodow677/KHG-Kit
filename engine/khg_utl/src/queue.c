#include "khg_utl/queue.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

utl_queue *queue_create(size_t itemSize) {
  if (itemSize == 0) {
    error_func("Item size must be greater than zero in queue_create", user_defined_data);
    exit(-1);
  }
  utl_queue *queue = (utl_queue *)malloc(sizeof(utl_queue));
  if (!queue) {
    error_func("Cannot allocate memory for queue in queue_create", user_defined_data);
    exit(-1);
  }
  queue->vec = vector_create(itemSize);
  if (!queue->vec) {
    error_func("Cannot allocate memory for queue->vec in queue_create", user_defined_data);
    free(queue);
    exit(-1);
  }
  return queue;
}

bool queue_empty(const utl_queue *q) {
  if (!q) {
    error_func("Queue pointer is NULL in queue_empty", user_defined_data);
    return true;
  }
  if (!q->vec) {
    error_func("Queue vector is NULL in queue_empty", user_defined_data);
    return true;
  }
  return vector_is_empty(q->vec);
}

size_t queue_size(const utl_queue *q) {
  if (!q) {
    error_func("Queue pointer is NULL in queue_size", user_defined_data);
    return 0;
  }
  if (!q->vec) {
    error_func("Queue vector is NULL in queue_size", user_defined_data);
    return 0;
  }
  return vector_size(q->vec);
}

void queue_push(utl_queue *q, void *item) {
  if (!q) {
    error_func("Queue is NULL in queue_push", user_defined_data);
    return;
  }
  if (!q->vec) {
    error_func("Vector is NULL in queue_push", user_defined_data);
    return;
  }
  if (!item) {
    error_func("Item to push is NULL in queue_push", user_defined_data);
    return;
  }
  vector_push_back(q->vec, item);
}

void *queue_front(const utl_queue *q) {
  if (!q) {
    error_func("Queue pointer is NULL in queue_front", user_defined_data);
    return NULL;
  }
  if (!q->vec) {
    error_func("Queue's vector is NULL in queue_front", user_defined_data);
    return NULL;
  }
  if (vector_is_empty(q->vec)) {
    error_func("Queue is empty in queue_front", user_defined_data);
    return NULL;
  }
  return vector_front(q->vec);
}

void *queue_back(const utl_queue *q){
  if (!q) {
    error_func("Queue pointer is NULL in queue_back", user_defined_data);
    return NULL;
  }
  if (!q->vec) {
    error_func("Queue's vector is NULL in queue_back", user_defined_data);
    return NULL;
  }
  if (vector_is_empty(q->vec)) {
    error_func("Queue is empty in queue_back", user_defined_data);
    return NULL;
  }
  return vector_back(q->vec);
}

void queue_pop(utl_queue *q) {
  if (!q) {
    error_func("Queue pointer is NULL in queue_pop", user_defined_data);
    return;
  }
  if (!q->vec) {
    error_func("Queue's vector is NULL in queue_pop", user_defined_data);
    return;
  }
  if (vector_is_empty(q->vec)) {
    error_func("Queue is empty in queue_pop", user_defined_data);
    return;
  }
  vector_erase(q->vec, 0, 1);
}

void queue_emplace(utl_queue *q, void *item, size_t itemSize) {
  if (!q) {
    error_func("Queue pointer is NULL in queue_emplace", user_defined_data);
    return;
  }
  if (!q->vec) {
    error_func("Queue's vector is NULL in queue_emplace", user_defined_data);
    return;
  }
  if (!item) {
    error_func("Item pointer is NULL in queue_emplace", user_defined_data);
    return;
  }
  vector_emplace_back(q->vec, item, itemSize);
}

void queue_swap(utl_queue *q1, utl_queue *q2){
  if (!q1 || !q2) {
    error_func("One or both Queue pointers are NULL in queue_swap", user_defined_data);
    return;
  }
  utl_vector *tempVec = q1->vec;
  q1->vec = q2->vec;
  q2->vec = tempVec;
}

void queue_deallocate(utl_queue *q) {
  if (!q) {
    error_func("Queue pointer is NULL in queue_deallocate", user_defined_data);
    return;
  }
  if (q->vec) { 
    vector_deallocate(q->vec);
  }
  free(q);
}

bool queue_is_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    error_func("One or both Queue pointers are NULL in queue_is_equal", user_defined_data);
    return q1 == q2;
  }
  return vector_is_equal(q1->vec, q2->vec);
}

bool queue_is_less(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    error_func("One or both Queue pointers are NULL in queue_is_less", user_defined_data);
    return q1 != NULL && q2 == NULL;
  }
  return vector_is_less(q1->vec, q2->vec);
}

bool queue_is_greater(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    error_func("One or both Queue pointers are NULL in queue_is_greater", user_defined_data);
    return q1 == NULL && q2 != NULL;
  }
  return vector_is_greater(q1->vec, q2->vec);
}

bool queue_is_not_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    error_func("One or both Queue pointers are NULL in queue_is_not_equal", user_defined_data);
    return q1 != q2;
  }
  return !queue_is_equal(q1, q2);
}

bool queue_is_less_or_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    error_func("One or both Queue pointers are NULL in queue_is_less_or_equal", user_defined_data);
    return q1 != NULL || q2 == NULL;
  }
  return queue_is_less(q1, q2) || queue_is_equal(q1, q2);
}

bool queue_is_greater_or_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    error_func("One or both Queue pointers are NULL in queue_is_greater_or_equal", user_defined_data);
    return q1 == NULL || q2 != NULL;
  }
  return queue_is_greater(q1, q2) || queue_is_equal(q1, q2);
}

