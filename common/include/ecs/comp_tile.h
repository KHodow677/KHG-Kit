#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"

typedef struct comp_tile {
  ecs_id id;
  unsigned int tile_id;
  phy_vector2 tile_pos;
} comp_tile;

typedef struct comp_tile_constructor_info {
  unsigned int tile_id;
  phy_vector2 tile_pos;
} comp_tile_constructor_info;

extern ecs_id TILE_COMPONENT_SIGNATURE;
extern ecs_id TILE_SYSTEM_SIGNATURE;

extern comp_tile_constructor_info *TILE_CONSTRUCTOR_INFO;

void comp_tile_register(void);
void sys_tile_register(void);

comp_tile *sys_tile_add(const ecs_id eid, comp_tile_constructor_info *clci);
