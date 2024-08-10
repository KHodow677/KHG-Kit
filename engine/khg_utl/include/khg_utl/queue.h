#pragma once

#include "khg_utl/vector.h"

typedef struct {
  Vector *vec;
} Queue;

Queue *queue_create(size_t itemSize);
size_t queue_size(const Queue *q);

void *queue_front(const Queue *q);
void *queue_back(const Queue *q);

void queue_pop(Queue *q);
void queue_emplace(Queue *q, void *item, size_t itemSize);
void queue_swap(Queue *q1, Queue *q2);
void queue_deallocate(Queue *q); 
void queue_push(Queue *q, void *item);

bool queue_is_equal(const Queue *q1, const Queue *q2);
bool queue_is_less(const Queue *q1, const Queue *q2);
bool queue_is_greater(const Queue *q1, const Queue *q2);
bool queue_is_not_equal(const Queue *q1, const Queue *q2);
bool queue_is_less_or_equal(const Queue *q1, const Queue *q2);
bool queue_is_greater_or_equal(const Queue *q1, const Queue *q2);
bool queue_empty(const Queue *q);

