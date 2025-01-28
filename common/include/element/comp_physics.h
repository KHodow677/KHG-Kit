#pragma once

#include "element/namespace.h"
#include "khg_ecs/ecs.h"

void comp_physics_register(void);
void sys_physics_register(void);

element_comp_physics *sys_physics_add(const ecs_id eid, element_comp_physics *cp);

