#pragma once

#include "khg_utl/vector.h"

typedef struct {
  utl_vector *vec;
} utl_queue;

utl_queue *utl_queue_create(size_t itemSize);
size_t utl_queue_size(const utl_queue *q);

void *utl_queue_front(const utl_queue *q);
void *utl_queue_back(const utl_queue *q);

void utl_queue_pop(utl_queue *q);
void utl_queue_emplace(utl_queue *q, void *item, size_t itemSize);
void utl_queue_swap(utl_queue *q1, utl_queue *q2);
void utl_queue_deallocate(utl_queue *q); 
void utl_queue_push(utl_queue *q, void *item);

bool utl_queue_is_equal(const utl_queue *q1, const utl_queue *q2);
bool utl_queue_is_less(const utl_queue *q1, const utl_queue *q2);
bool utl_queue_is_greater(const utl_queue *q1, const utl_queue *q2);
bool utl_queue_is_not_equal(const utl_queue *q1, const utl_queue *q2);
bool utl_queue_is_less_or_equal(const utl_queue *q1, const utl_queue *q2);
bool utl_queue_is_greater_or_equal(const utl_queue *q1, const utl_queue *q2);
bool utl_queue_empty(const utl_queue *q);

