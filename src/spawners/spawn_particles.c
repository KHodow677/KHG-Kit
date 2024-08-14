#include "spawners/spawn_particles.h"
#include "data_utl/id_utl.h"
#include "entity/entity.h"
#include "entity/ecs_manager.h"
#include "generators/entities/particle_generator.h"
#include "khg_utl/map.h"
#include <stdio.h>
#include <stdlib.h>

void spawn_particle() {
  generic_entity *ge = malloc(sizeof(generic_entity));
  particle *p = malloc(sizeof(particle));
  generate_particle(p, ECS);
  ge->type = ENTITY_TYPE_PARTICLE;
  ge->particle = p;
  ge->particle->destroyer_info.unique_id = generate_unique_id();
  bool status = utl_map_insert(ENTITY_LOOKUP, &ge->particle->destroyer_info.unique_id, ge);
  printf("%B\n", status);
}

