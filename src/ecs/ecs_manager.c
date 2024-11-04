#include "ecs/ecs_manager.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_mover.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/comp_zone.h"
#include "khg_ecs/ecs.h"

ecs_ecs *ECS = NULL;

void ecs_setup() {
  ECS = ecs_new(ECS_ENTITY_COUNT, NULL);
  comp_physics_register();
  comp_renderer_register();
  comp_animator_register();
  comp_light_register();
  comp_zone_register();
  comp_mover_register();
  sys_physics_register();
  sys_renderer_register();
  sys_animator_register();
  sys_light_register();
  sys_zone_register();
  sys_mover_register();
}

void ecs_cleanup() {
  ecs_free(ECS);
}

