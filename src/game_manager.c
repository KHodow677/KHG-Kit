#include "game_manager.h"
#include "entity/camera.h"
#include "entity/comp_mover.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_shooter.h"
#include "entity/entity.h"
#include "generators/components/texture_generator.h"
#include "physics/physics_setup.h"
#include "threading/thread_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdlib.h>
#include <stdio.h>

phy_space *SPACE;
ecs_ecs *ECS;
utl_vector *ENTITY_LOOKUP;
utl_vector *TEXTURE_LOOKUP;
thd_thread *WORKER_THREADS;

gfx_texture NO_TEXTURE = { 0 };
int MAX_TEXTURES = 1024;
int CURRENT_TEXTURE_ID = 0;
int THREAD_COUNT;
camera CAMERA = { 0 };

sys_physics PHYSICS_SYSTEM = { 0 };
sys_renderer RENDERER_SYSTEM = { 0 };
sys_destroyer DESTROYER_SYSTEM = { 0 };
sys_animator ANIMATOR_SYSTEM = { 0 };
sys_mover MOVER_SYSTEM = { 0 };
sys_rotator ROTATOR_SYSTEM = { 0 };
sys_shooter SHOOTER_SYSTEM = { 0 };

comp_physics PHYSICS_COMPONENT_TYPE;
comp_renderer RENDERER_COMPONENT_TYPE;
comp_destroyer DESTROYER_COMPONENT_TYPE;
comp_animator ANIMATOR_COMPONENT_TYPE;
comp_mover MOVER_COMPONENT_TYPE;
comp_rotator ROTATOR_COMPONENT_TYPE;
comp_shooter SHOOTER_COMPONENT_TYPE;

void ecs_setup() {
  camera_setup(&CAMERA);
  ECS = ecs_new(1024, NULL);
  comp_physics_register(&PHYSICS_COMPONENT_TYPE);
  comp_renderer_register(&RENDERER_COMPONENT_TYPE);
  comp_destroyer_register(&DESTROYER_COMPONENT_TYPE);
  comp_animator_register(&ANIMATOR_COMPONENT_TYPE);
  comp_mover_register(&MOVER_COMPONENT_TYPE);
  comp_rotator_register(&ROTATOR_COMPONENT_TYPE);
  comp_shooter_register(&SHOOTER_COMPONENT_TYPE);
  sys_physics_register(&PHYSICS_SYSTEM);
  sys_renderer_register(&RENDERER_SYSTEM);
  sys_destroyer_register(&DESTROYER_SYSTEM);
  sys_animator_register(&ANIMATOR_SYSTEM);
  sys_mover_register(&MOVER_SYSTEM);
  sys_rotator_register(&ROTATOR_SYSTEM);
  sys_shooter_register(&SHOOTER_SYSTEM);
  generate_entity_lookup();
  generate_textures();
}

void ecs_cleanup() {
  sys_physics_free(false);
  sys_renderer_free(false);
  sys_destroyer_free(false);
  sys_mover_free(false);
  sys_rotator_free(false);
  sys_shooter_free(false);
  free_entity_lookup();
  free_textures();
  physics_free(SPACE);
  ecs_free(ECS);
  free_worker_threads();
}

