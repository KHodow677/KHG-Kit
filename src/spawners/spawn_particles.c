#include "spawners/spawn_particles.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/particle_generator.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

void spawn_particle(phy_body *body_body, phy_body *top_body, float x, float y) {
  generic_entity *ge = malloc(sizeof(generic_entity));
  ge->type = ENTITY_TYPE_PARTICLE;
  ge->particle = (particle){ 0 };
  generate_particle(&ge->particle, body_body, top_body, x, y);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
}

