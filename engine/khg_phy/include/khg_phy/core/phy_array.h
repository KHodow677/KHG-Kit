#pragma once

#include <stdlib.h>

typedef struct phy_array {
  size_t size;
  size_t max;
  float growth_factor;
  void **data;
} phy_array;

typedef void (*phy_array_free_each_callback)(void *);

phy_array *phy_array_new();
phy_array *phy_array_new_ex(size_t default_capacity, float growth_factor);
void phy_array_free(phy_array *array);
void phy_array_free_each(phy_array *array, phy_array_free_each_callback free_func);

int phy_array_add(phy_array *array, void *elem);
void *phy_array_pop(phy_array *array, size_t index);
size_t phy_array_remove(phy_array *array, void *elem);
int phy_array_clear(phy_array *array, void (free_func)(void *));

