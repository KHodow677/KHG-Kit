#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

typedef enum {
  TARGET_FIRST, 
  TARGET_LAST, 
  TARGET_STRONG, 
  TARGET_WEAK, 
} targeting_mode;

typedef struct {
  ecs_id id;
  float range;
  targeting_mode mode;
  phy_shape *sensor;
  utl_vector *all_list;
} comp_targeter;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_targeter;

extern ecs_id TARGETER_COMPONENT_SIGNATURE;

bool targeter_sensor_enter(phy_arbiter *arb, phy_space *space, phy_data_pointer udata);
void targeter_sensor_exit(phy_arbiter *arb, phy_space *space, phy_data_pointer udata);

void comp_targeter_register(void);

void sys_targeter_register(sys_targeter *ss);
comp_targeter *sys_targeter_add(ecs_id eid);

