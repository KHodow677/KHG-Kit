#include "spawners/spawn_particles.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/particle_generator.h"
#include "khg_utl/vector.h"

generic_entity *spawn_particle(physics_info *p_info, float x, float y) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_PARTICLE;
  stored_ge->particle = (particle){ 0 };
  generate_particle(&stored_ge->particle, p_info, x, y);
  return stored_ge;
}

