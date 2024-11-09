#pragma once

#include "khg_ecs/ecs.h"

#define ECS_ENTITY_COUNT 1024

extern ecs_ecs *ECS;

void ecs_setup(void);
void ecs_cleanup(void);

