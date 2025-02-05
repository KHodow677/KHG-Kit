#pragma once

#include "tasking/namespace.h"
#include <stdbool.h>

void load_thread_defer(resource_thread *resource, int (*task)(void *), resource_thread *await);
void load_resources_defer(void);
