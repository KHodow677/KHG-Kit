#include "khg_utl/queue.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

utl_queue *utl_queue_create(size_t itemSize) {
  if (itemSize == 0) {
    utl_error_func("Item size must be greater than zero in queue_create", utl_user_defined_data);
    exit(-1);
  }
  utl_queue *queue = (utl_queue *)malloc(sizeof(utl_queue));
  if (!queue) {
    utl_error_func("Cannot allocate memory for queue in queue_create", utl_user_defined_data);
    exit(-1);
  }
  queue->vec = utl_vector_create(itemSize);
  if (!queue->vec) {
    utl_error_func("Cannot allocate memory for queue->vec in queue_create", utl_user_defined_data);
    free(queue);
    exit(-1);
  }
  return queue;
}

bool utl_queue_empty(const utl_queue *q) {
  if (!q) {
    utl_error_func("Queue pointer is NULL in queue_empty", utl_user_defined_data);
    return true;
  }
  if (!q->vec) {
    utl_error_func("Queue vector is NULL in queue_empty", utl_user_defined_data);
    return true;
  }
  return utl_vector_is_empty(q->vec);
}

size_t utl_queue_size(const utl_queue *q) {
  if (!q) {
    utl_error_func("Queue pointer is NULL in queue_size", utl_user_defined_data);
    return 0;
  }
  if (!q->vec) {
    utl_error_func("Queue vector is NULL in queue_size", utl_user_defined_data);
    return 0;
  }
  return utl_vector_size(q->vec);
}

void utl_queue_push(utl_queue *q, void *item) {
  if (!q) {
    utl_error_func("Queue is NULL in queue_push", utl_user_defined_data);
    return;
  }
  if (!q->vec) {
    utl_error_func("Vector is NULL in queue_push", utl_user_defined_data);
    return;
  }
  if (!item) {
    utl_error_func("Item to push is NULL in queue_push", utl_user_defined_data);
    return;
  }
  utl_vector_push_back(q->vec, item);
}

void *utl_queue_front(const utl_queue *q) {
  if (!q) {
    utl_error_func("Queue pointer is NULL in queue_front", utl_user_defined_data);
    return NULL;
  }
  if (!q->vec) {
    utl_error_func("Queue's vector is NULL in queue_front", utl_user_defined_data);
    return NULL;
  }
  if (utl_vector_is_empty(q->vec)) {
    utl_error_func("Queue is empty in queue_front", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_front(q->vec);
}

void *utl_queue_back(const utl_queue *q){
  if (!q) {
    utl_error_func("Queue pointer is NULL in queue_back", utl_user_defined_data);
    return NULL;
  }
  if (!q->vec) {
    utl_error_func("Queue's vector is NULL in queue_back", utl_user_defined_data);
    return NULL;
  }
  if (utl_vector_is_empty(q->vec)) {
    utl_error_func("Queue is empty in queue_back", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_back(q->vec);
}

void utl_queue_pop(utl_queue *q) {
  if (!q) {
    utl_error_func("Queue pointer is NULL in queue_pop", utl_user_defined_data);
    return;
  }
  if (!q->vec) {
    utl_error_func("Queue's vector is NULL in queue_pop", utl_user_defined_data);
    return;
  }
  if (utl_vector_is_empty(q->vec)) {
    utl_error_func("Queue is empty in queue_pop", utl_user_defined_data);
    return;
  }
  utl_vector_erase(q->vec, 0, 1);
}

void utl_queue_emplace(utl_queue *q, void *item, size_t itemSize) {
  if (!q) {
    utl_error_func("Queue pointer is NULL in queue_emplace", utl_user_defined_data);
    return;
  }
  if (!q->vec) {
    utl_error_func("Queue's vector is NULL in queue_emplace", utl_user_defined_data);
    return;
  }
  if (!item) {
    utl_error_func("Item pointer is NULL in queue_emplace", utl_user_defined_data);
    return;
  }
  utl_vector_emplace_back(q->vec, item, itemSize);
}

void utl_queue_swap(utl_queue *q1, utl_queue *q2){
  if (!q1 || !q2) {
    utl_error_func("One or both Queue pointers are NULL in queue_swap", utl_user_defined_data);
    return;
  }
  utl_vector *tempVec = q1->vec;
  q1->vec = q2->vec;
  q2->vec = tempVec;
}

void utl_queue_deallocate(utl_queue *q) {
  if (!q) {
    utl_error_func("Queue pointer is NULL in queue_deallocate", utl_user_defined_data);
    return;
  }
  if (q->vec) { 
    utl_vector_deallocate(q->vec);
  }
  free(q);
}

bool utl_queue_is_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    utl_error_func("One or both Queue pointers are NULL in queue_is_equal", utl_user_defined_data);
    return q1 == q2;
  }
  return utl_vector_is_equal(q1->vec, q2->vec);
}

bool utl_queue_is_less(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    utl_error_func("One or both Queue pointers are NULL in queue_is_less", utl_user_defined_data);
    return q1 != NULL && q2 == NULL;
  }
  return utl_vector_is_less(q1->vec, q2->vec);
}

bool utl_queue_is_greater(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    utl_error_func("One or both Queue pointers are NULL in queue_is_greater", utl_user_defined_data);
    return q1 == NULL && q2 != NULL;
  }
  return utl_vector_is_greater(q1->vec, q2->vec);
}

bool utl_queue_is_not_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    utl_error_func("One or both Queue pointers are NULL in queue_is_not_equal", utl_user_defined_data);
    return q1 != q2;
  }
  return !utl_queue_is_equal(q1, q2);
}

bool utl_queue_is_less_or_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    utl_error_func("One or both Queue pointers are NULL in queue_is_less_or_equal", utl_user_defined_data);
    return q1 != NULL || q2 == NULL;
  }
  return utl_queue_is_less(q1, q2) || utl_queue_is_equal(q1, q2);
}

bool utl_queue_is_greater_or_equal(const utl_queue *q1, const utl_queue *q2) {
  if (!q1 || !q2) {
    utl_error_func("One or both Queue pointers are NULL in queue_is_greater_or_equal", utl_user_defined_data);
    return q1 == NULL || q2 != NULL;
  }
  return utl_queue_is_greater(q1, q2) || utl_queue_is_equal(q1, q2);
}

