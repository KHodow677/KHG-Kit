#pragma once

#include "element/namespace.h"
#include "khg_ecs/ecs.h"

void comp_light_register(void);
void sys_light_register(void);

comp_light *sys_light_add(const ecs_id eid, comp_light *cl);

