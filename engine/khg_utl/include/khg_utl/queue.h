#pragma once

#include "khg_utl/vector.h"

typedef struct {
  utl_vector *vec;
} utl_queue;

utl_queue *queue_create(size_t itemSize);
size_t queue_size(const utl_queue *q);

void *queue_front(const utl_queue *q);
void *queue_back(const utl_queue *q);

void queue_pop(utl_queue *q);
void queue_emplace(utl_queue *q, void *item, size_t itemSize);
void queue_swap(utl_queue *q1, utl_queue *q2);
void queue_deallocate(utl_queue *q); 
void queue_push(utl_queue *q, void *item);

bool queue_is_equal(const utl_queue *q1, const utl_queue *q2);
bool queue_is_less(const utl_queue *q1, const utl_queue *q2);
bool queue_is_greater(const utl_queue *q1, const utl_queue *q2);
bool queue_is_not_equal(const utl_queue *q1, const utl_queue *q2);
bool queue_is_less_or_equal(const utl_queue *q1, const utl_queue *q2);
bool queue_is_greater_or_equal(const utl_queue *q1, const utl_queue *q2);
bool queue_empty(const utl_queue *q);

