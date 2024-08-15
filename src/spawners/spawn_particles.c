#include "spawners/spawn_particles.h"
#include "entity/comp_physics.h"
#include "entity/entity.h"
#include "entity/ecs_manager.h"
#include "generators/entities/particle_generator.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

void spawn_particle(physics_info *source_info, float x, float y) {
  generic_entity *ge = malloc(sizeof(generic_entity));
  ge->type = ENTITY_TYPE_PARTICLE;
  ge->particle = (particle){ 0 };
  generate_particle(&ge->particle, source_info, ECS, x, y);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
}

