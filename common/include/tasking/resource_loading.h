#pragma once

#include "tasking/namespace.h"
#include <stdbool.h>

void load_configs(const char *tex_filename);
void close_configs(void);

void load_thread_defer(tasking_resource_thread *resource, int (*task)(void *), tasking_resource_thread *await);
void load_resources_defer(void);

