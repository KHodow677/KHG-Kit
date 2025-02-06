#pragma once

#include "element/namespace.h"
#include "khg_ecs/ecs.h"

void comp_tile_register(void);
void sys_tile_register(void);

comp_tile *sys_tile_add(const ecs_id eid, comp_tile *ct);
