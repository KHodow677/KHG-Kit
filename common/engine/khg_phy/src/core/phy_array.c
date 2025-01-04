#include "khg_phy/core/phy_array.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

phy_array *phy_array_new() {
  phy_array *array = malloc(sizeof(phy_array));
  if (!array) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  array->size = 0;
  array->max = 1;
  array->growth_factor = 2.0;
  array->data = (void **)malloc(sizeof(void *));
  if (!array->data) free(array);
  if (!array->data) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  return array;
}

phy_array *phy_array_new_ex(unsigned int default_capacity, float growth_factor) {
  phy_array *array = malloc(sizeof(phy_array));
  if (!array) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  array->size = 0;
  array->max = default_capacity;
  array->growth_factor = growth_factor;
  array->data = (void **)malloc(sizeof(void *) * default_capacity);
  if (!array->data) {
    free(array);
  }
  if (!array->data) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  return array;
}

void phy_array_free(phy_array *array) {
  if (!array) {
    return;
  }
  free(array->data);
  free(array);
}

void phy_array_free_each(phy_array *array, phy_array_free_each_callback free_func) {
  for (unsigned int i = 0; i < array->size; i++) {
    free_func(array->data[i]);
  }
}

int phy_array_add(phy_array *array, void *elem) {
  if (array->size == array->max) {
    array->size++;
    array->max = (unsigned int)((float)array->max * array->growth_factor);
    array->data = realloc(array->data, array->max * sizeof(void *));
    if (!array->data) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }
  }
  else {
    array->size++;
  }
  array->data[array->size - 1] = elem;
  return 0;
}

void *phy_array_pop(phy_array *array, unsigned int index) {
  for (unsigned int i = 0; i < array->size; i++) {
    if (i == index) {
      array->size--;
      void *elem = array->data[i];
      array->data[i] = array->data[array->size];
      array->data[array->size] = NULL;
      return elem;
    }
  }
  return NULL;
}

unsigned int phy_array_remove(phy_array *array, void *elem) {
  for (unsigned int i = 0; i < array->size; i++) {
    if (array->data[i] == elem) {
      array->size--;
      array->data[i] = array->data[array->size];
      array->data[array->size] = NULL;
      return i;
    }
  }
  return -1;
}

int phy_array_clear(phy_array *array, void (free_func)(void *)) {
  if (array->size == 0) {
    return 0;
  }
  if (!free_func) {
    while (array->size > 0) {
      if (!phy_array_pop(array, 0)) {
        return 1;
      }
    }
  }
  else {
    while (array->size > 0) {
      void *p = phy_array_pop(array, 0);
      if (!p) {
        return 1;
      }
      free_func(p);
    }
  }
  return 0;
}

