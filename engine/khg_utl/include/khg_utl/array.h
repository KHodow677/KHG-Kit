#pragma once

#include "khg_utl/vector.h"

typedef struct utl_array {
  utl_vector *vec;
} utl_array;

utl_array *utl_array_create(size_t element_size, size_t size);

bool utl_array_is_equal(const utl_array *arr1, const utl_array *arr2);
bool utl_array_is_less(const utl_array *arr1, const utl_array *arr2);
bool utl_array_is_greater(const utl_array *arr1, const utl_array *arr2);
bool utl_array_is_not_equal(const utl_array *arr1, const utl_array *arr2);
bool utl_array_is_less_or_equal(const utl_array *arr1, const utl_array *arr2);
bool utl_array_is_greater_or_equal(const utl_array *arr1, const utl_array *arr2);
bool utl_array_empty(utl_array *arr);

void utl_array_deallocate(utl_array *arr);
void utl_array_set(utl_array *arr, size_t index, const void *value);
void utl_array_insert(utl_array *mainArr, const utl_array *otherArr, size_t index);
void utl_array_fill(utl_array *arr, const void *value);
void utl_array_swap(utl_array *arr1, utl_array *arr2);

void *utl_array_at(utl_array *arr, size_t index);
void *utl_array_begin(utl_array *arr);
void *utl_array_end(utl_array *arr);
void *utl_array_rbegin(utl_array *arr);
void *utl_array_rend(utl_array *arr);
void *utl_array_front(utl_array *arr);
void *utl_array_back(utl_array *arr);
void *utl_array_data(utl_array *arr);

size_t utl_array_size(utl_array *arr);
size_t utl_array_max_size(utl_array *arr);

const void *utl_array_cbegin(utl_array *arr);
const void *utl_array_cend(utl_array *arr);
const void *utl_array_crbegin(utl_array *arr);
const void *utl_array_crend(utl_array *arr);

void utl_array_clear(utl_array *arr);
void utl_array_reverse(utl_array *arr);
void utl_array_sort(utl_array *arr, int (*compare)(const void*, const void*));
void utl_array_copy(utl_array *dest, const utl_array *src);

