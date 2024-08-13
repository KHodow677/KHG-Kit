#include "entity/ecs_manager.h"
#include "game.h"
#include "data_utl/map_utl.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/components/texture_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "khg_gfx/texture.h"
#include "khg_utl/map.h"
#include "khg_utl/vector.h"
#include "physics/physics_setup.h"
#include "khg_ecs/ecs.h"
#include <stdlib.h>
#include <stdio.h>

ecs_ecs *ECS;
utl_map *ENTITY_LOOKUP;
utl_vector *TEXTURE_LOOKUP;

sys_physics PHYSICS_SYSTEM = { 0 };
sys_renderer RENDERER_SYSTEM = { 0 };
sys_follower FOLLOWER_SYSTEM = { 0 };
sys_destroyer DESTROYER_SYSTEM = { 0 };
sys_animator ANIMATOR_SYSTEM = { 0 };

void ecs_setup(comp_physics *cp, comp_renderer *cr, comp_follower *cf, comp_destroyer *cd, comp_animator *ca, tank_body *tb, tank_top *tt) {
  ECS = ecs_new(1024, NULL);
  comp_physics_register(cp, ECS);
  comp_renderer_register(cr, ECS);
  comp_follower_register(cf, ECS);
  comp_destroyer_register(cd, ECS);
  comp_animator_register(ca, ECS);
  sys_physics_register(&PHYSICS_SYSTEM, ECS);
  sys_renderer_register(&RENDERER_SYSTEM, ECS);
  sys_follower_register(&FOLLOWER_SYSTEM, ECS);
  sys_destroyer_register(&DESTROYER_SYSTEM, ECS);
  sys_animator_register(&ANIMATOR_SYSTEM, ECS);
  ENTITY_LOOKUP = utl_map_create(compare_ints, free_deallocator, free_entity_deallocator);
  TEXTURE_LOOKUP = utl_vector_create(sizeof(gfx_texture *));

  generate_textures();

  generate_tank_body(tb, ECS);
  generate_tank_top(tt, ECS, tb);
}

void ecs_cleanup(tank_body *tb, tank_top *tt) {
  if (!tb->destroyer_info.destroy_now) {
    free_tank_body(tb);
  }
  if (!tt->destroyer_info.destroy_now) {
    free_tank_top(tt);
  }
  sys_physics_free(false);
  sys_renderer_free(false);
  sys_follower_free(false);
  sys_destroyer_free(false);
  free_textures();
  utl_map_deallocate(ENTITY_LOOKUP);
  physics_free(SPACE);
  ecs_free(ECS);
}

