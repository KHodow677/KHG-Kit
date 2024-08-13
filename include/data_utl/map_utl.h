#pragma once

#include "khg_utl/map.h"

int compare_ints(const key_type a, const key_type b);
void no_deallocator(void *data);
void free_deallocator(void *data);
void free_entity_deallocator(void *data);

