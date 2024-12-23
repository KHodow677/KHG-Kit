#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/contact.h"
#include "physics/physics.h"

typedef enum move_direction {
  MOVE_LEFT,
  MOVE_RIGHT
} move_direction;

typedef struct comp_mover {
  ecs_id id;
  phy_contact_listener listener;
  move_direction current_direction;
  float left_current_speed;
  float right_current_speed;
  float max_speed;
  float time_to_max_speed;
  float left_pressed_time;
  float right_pressed_time;
} comp_mover;

typedef struct comp_mover_constructor_info {
  phy_rigid_body *body; 
  const float max_speed;
  const float time_to_max_speed;
} comp_mover_constructor_info;

extern ecs_id MOVER_COMPONENT_SIGNATURE;
extern ecs_id MOVER_SYSTEM_SIGNATURE;

extern comp_mover_constructor_info *MOVER_CONSTRUCTOR_INFO;

void mover_on_collision_added(phy_contact_event event, physics_collision_info *collision_info);

void comp_mover_register(void);
void sys_mover_register(void);

comp_mover *sys_mover_add(const ecs_id eid, comp_mover_constructor_info *clci);

