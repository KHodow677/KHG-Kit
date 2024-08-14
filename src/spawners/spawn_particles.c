#include "spawners/spawn_particles.h"
#include "entity/entity.h"
#include "entity/ecs_manager.h"
#include "generators/entities/particle_generator.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

void spawn_particle() {
  generic_entity *ge = malloc(sizeof(generic_entity));
  particle *p = malloc(sizeof(particle));
  generate_particle(p, ECS);
  ge->type = ENTITY_TYPE_PARTICLE;
  ge->particle = p;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
}

