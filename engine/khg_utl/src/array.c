#include "khg_utl/array.h"
#include "khg_utl/error_func.h"
#include <string.h>
#include <stdlib.h>

utl_array *utl_array_create(size_t element_size, size_t size) {
  utl_array *arr = (utl_array *)malloc(sizeof(utl_array));
  if (!arr) {
    utl_error_func("Cannot allocate memory", utl_user_defined_data);
    exit(-1);
  }
  arr->vec = utl_vector_create(element_size);
  if (!arr->vec) {
    utl_error_func("Cannot allocate memory", utl_user_defined_data);
    free(arr);
    exit(-1);
  }
  utl_vector_resize(arr->vec, size);
  return arr;
}

bool utl_array_is_equal(const utl_array *arr1, const utl_array *arr2) {
  if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) {
    return false;
  }
  if (arr1->vec->size != arr2->vec->size) {
    return false;
  }
  bool result = memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) == 0;
  return result;
}

bool utl_array_is_less(const utl_array *arr1, const utl_array *arr2) {
  if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) {
    utl_error_func("One or both arrays are null or their vectors are null", utl_user_defined_data);
    return false;
  }
  size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
  int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);
  bool result = cmp < 0 || (cmp == 0 && arr1->vec->size < arr2->vec->size);
  return result;
}

bool utl_array_is_greater(const utl_array *arr1, const utl_array *arr2) {
  return utl_array_is_less(arr2, arr1);
}

bool utl_array_is_not_equal(const utl_array *arr1, const utl_array *arr2) {
  if (arr1 == NULL || arr2 == NULL) {
    return arr1 != arr2;
  }
  if (arr1->vec->size != arr2->vec->size) {
    return true;
  }
  bool result = memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) != 0;
  return result;
}

bool utl_array_is_less_or_equal(const utl_array *arr1, const utl_array *arr2) {
  if (arr1 == NULL || arr2 == NULL) {
    return (arr1 == NULL) && (arr2 != NULL);
  }
  size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
  int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);
  bool result = cmp < 0 || (cmp == 0 && arr1->vec->size <= arr2->vec->size);
  return result;
}

bool utl_array_is_greater_or_equal(const utl_array *arr1, const utl_array *arr2) {
  if (arr1 == NULL && arr2 == NULL) {
    return true;
  }
  if (arr1 == NULL || arr2 == NULL) {
    return (arr1 != NULL) && (arr2 == NULL);
  }
  size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
  int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);
  bool result = cmp > 0 || (cmp == 0 && arr1->vec->size >= arr2->vec->size);
  return result;
}

bool utl_array_empty(utl_array *arr) {
  bool result = arr == NULL || arr->vec == NULL || arr->vec->size == 0;
  return result;
}

void utl_array_deallocate(utl_array *arr) {
  if (arr != NULL) {
    if (arr->vec != NULL) {
      utl_vector_deallocate(arr->vec);
    }
    free(arr);
  } 
}

void utl_array_set(utl_array *arr, size_t index, const void *value) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return;
  }
  if (index >= arr->vec->size) {
    utl_error_func("Index out of bounds", utl_user_defined_data);
    return;
  }
  if (value == NULL) {
    utl_error_func("Value is null", utl_user_defined_data);
    return;
  }
  memcpy((char *)arr->vec->items + (index * arr->vec->itemSize), value, arr->vec->itemSize);
}

void utl_array_insert(utl_array *mainArr, const utl_array *otherArr, size_t index) {
  if (mainArr == NULL) {
    utl_error_func("Main array is null", utl_user_defined_data);
    return;
  }
  if (mainArr->vec == NULL) {
    utl_error_func("Main array's vector is null", utl_user_defined_data);
    return;
  }
  if (otherArr == NULL) {
    utl_error_func("Other array is null", utl_user_defined_data);
    return;
  }
  if (otherArr->vec == NULL) {
    utl_error_func("Other array's vector is null", utl_user_defined_data);
    return;
  }
  size_t newTotalSize = index + otherArr->vec->size;
  if (newTotalSize > mainArr->vec->size) {
    utl_vector_resize(mainArr->vec, newTotalSize);
  }
  for (size_t i = 0; i < otherArr->vec->size; ++i) {
    void *value = utl_vector_at(otherArr->vec, i);
    memcpy((char *)mainArr->vec->items + ((index + i) * mainArr->vec->itemSize), value, mainArr->vec->itemSize);
  }
}

void utl_array_fill(utl_array *arr, const void *value) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return;
  }
  if (value == NULL) {
    utl_error_func("Value is null", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < arr->vec->size; ++i) { 
    memcpy((char *)arr->vec->items + (i * arr->vec->itemSize), value, arr->vec->itemSize);
  }
}

void utl_array_swap(utl_array *arr1, utl_array *arr2) {
  if (arr1 == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return;
  }
  if (arr2 == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return;
  }
  if (arr1->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return;
  }
  if (arr2->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return;
  }
  utl_vector *temp = arr1->vec;
  arr1->vec = arr2->vec;
  arr2->vec = temp;
}

void *utl_array_at(utl_array *arr, size_t index) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (index >= arr->vec->size) {
    utl_error_func("Index out of bounds", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_at(arr->vec, index);
}

void *utl_array_begin(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array is empty", utl_user_defined_data);
    return NULL;
  }
  return arr->vec->items;
}

void *utl_array_end(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array is empty", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_end(arr->vec);
}

void *utl_array_rbegin(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array is empty", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_rbegin(arr->vec);
}

void *utl_array_rend(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_rend(arr->vec);
}

void *utl_array_front(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array is empty", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_at(arr->vec, 0);
}

void *utl_array_back(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array is empty", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_at(arr->vec, arr->vec->size - 1);
}

void *utl_array_data(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  return arr->vec->items;
}

size_t utl_array_size(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return 0;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return 0;
  }
  return arr->vec->size;
}

size_t utl_array_max_size(utl_array *arr) {
  if (arr == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return 0;
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return 0;
  }
  return utl_vector_max_size(arr->vec);
}

const void *utl_array_cbegin(utl_array *arr) {
  if (arr == NULL) { 
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL; 
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array's vector size is 0", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_cbegin(arr->vec); 
}

const void *utl_array_cend(utl_array *arr) {
  if (arr == NULL) { 
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;  
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array's vector size is 0", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_cend(arr->vec);
}

const void *utl_array_crbegin(utl_array *arr) {
  if (arr == NULL) { 
    utl_error_func("Array is null", utl_user_defined_data);
    return NULL;  
  }
  if (arr->vec == NULL) {
    utl_error_func("Array's vector is null", utl_user_defined_data);
    return NULL;
  }
  if (arr->vec->size == 0) {
    utl_error_func("Array's vector size is 0", utl_user_defined_data);
    return NULL;
  }
  return utl_vector_crbegin(arr->vec);
}

const void *utl_array_crend(utl_array *arr) {
  if (arr == NULL || arr->vec == NULL) { 
    utl_error_func("Invalid input", utl_user_defined_data);
    return NULL;  
  }
  return utl_vector_crend(arr->vec);
}

void utl_array_clear(utl_array *arr) {
  if (arr == NULL || arr->vec == NULL) { 
    utl_error_func("Invalid array or vector pointer", utl_user_defined_data);
    return;
  }
  utl_vector_clear(arr->vec);
}

void utl_array_reverse(utl_array *arr) {
  if (arr == NULL || arr->vec == NULL) { 
    utl_error_func("Invalid array or vector pointer", utl_user_defined_data);
    return;
  }
  if (arr->vec->size <= 1) {
    return;
  }
  size_t start = 0;
  size_t end = arr->vec->size - 1;
  char *temp = (char *)malloc(arr->vec->itemSize);
  if (!temp) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  while (start < end) {
    memcpy(temp, (char *)arr->vec->items + (start * arr->vec->itemSize), arr->vec->itemSize);
    memcpy((char *)arr->vec->items + (start * arr->vec->itemSize), (char *)arr->vec->items + (end * arr->vec->itemSize), arr->vec->itemSize);
    memcpy((char *)arr->vec->items + (end * arr->vec->itemSize), temp, arr->vec->itemSize);
    start++;
    end--;
  }
  free(temp);
}

void utl_array_sort(utl_array *arr, int (*compare)(const void*, const void*)) {
  if (arr == NULL || arr->vec == NULL || compare == NULL) {
    utl_error_func("Invalid array, vector pointer, or compare function", utl_user_defined_data);
    return;
  }
  if (arr->vec->size <= 1) {
    return;
  }
  qsort(arr->vec->items, arr->vec->size, arr->vec->itemSize, compare);
}

void utl_array_copy(utl_array *dest, const utl_array *src) {
  if (src == NULL) {
    utl_error_func("Source array is null", utl_user_defined_data);
    return;
  }
  if (dest == NULL) {
    utl_error_func("Destination array is null", utl_user_defined_data);
    return;
  }
  utl_vector_resize(dest->vec, src->vec->size);
  memcpy(dest->vec->items, src->vec->items, src->vec->size * src->vec->itemSize);
}

