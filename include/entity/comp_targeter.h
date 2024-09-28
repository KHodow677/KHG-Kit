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
  float range;
  targeting_mode mode;
  phy_shape *sensor;
  utl_vector *all_list;
} targeter_info;

typedef struct {
  ecs_id id;
} comp_targeter;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_targeter;

extern ecs_id TARGETER_COMPONENT_SIGNATURE;
extern targeter_info NO_TARGETER;
extern targeter_info *TARGETER_INFO;

bool targeter_sensor_enter(phy_arbiter *arb, phy_space *space, phy_data_pointer udata);
void targeter_sensor_exit(phy_arbiter *arb, phy_space *space, phy_data_pointer udata);

void comp_targeter_register(comp_targeter *ct);

void sys_targeter_register(sys_targeter *ss);
void sys_targeter_add(ecs_id *eid, targeter_info *info);

