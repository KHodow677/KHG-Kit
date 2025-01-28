#pragma once

#define NAMESPACE_LOADING_IMPL

#include "loading/namespace.h"
#include <stdbool.h>

void load_thread_defer(resource_thread *resource, int (*task)(void *));
void load_resources_defer(void);

