#pragma once

#include "element/namespace.h"
#include "khg_ecs/ecs.h"

void comp_render_register(void);
void sys_render_register(void);

element_comp_render *sys_render_add(const ecs_id eid, element_comp_render *cr);

