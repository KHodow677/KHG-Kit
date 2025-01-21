#pragma once

#include "area/ovr_tile.h"
#include "khg_ecs/ecs.h"

typedef struct comp_tile {
  ecs_id id;
  ovr_tile_info tile;
} comp_tile;

typedef struct comp_tile_constructor_info {
  ovr_tile_info tile;
} comp_tile_constructor_info;

extern ecs_id TILE_COMPONENT_SIGNATURE;
extern ecs_id TILE_SYSTEM_SIGNATURE;

extern comp_tile_constructor_info *TILE_CONSTRUCTOR_INFO;

void comp_tile_register(void);
void sys_tile_register(void);

comp_tile *sys_tile_add(const ecs_id eid, comp_tile_constructor_info *clci);

