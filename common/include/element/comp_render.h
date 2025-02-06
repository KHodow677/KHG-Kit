#pragma once

#include "element/namespace.h"
#include "khg_ecs/ecs.h"

void comp_render_register(void);
void sys_render_register(void);

comp_render *sys_render_add(const ecs_id eid, comp_render *cr);

