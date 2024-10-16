#include "ecs/ecs_manager.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "khg_ecs/ecs.h"

ecs_ecs *ECS = NULL;

void ecs_setup() {
  ECS = ecs_new(ECS_ENTITY_COUNT, NULL);
  comp_physics_register();
  comp_renderer_register();
  sys_physics_register();
  sys_renderer_register();
}

void ecs_cleanup() {
  ecs_free(ECS);
}
