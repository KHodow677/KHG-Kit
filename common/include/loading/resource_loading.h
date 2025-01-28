#pragma once

#include "loading/namespace.h"
#include <stdbool.h>

void load_thread_defer(loading_resource_thread *resource, int (*task)(void *));
void load_resources_defer(void);

