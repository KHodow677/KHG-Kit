#include "spawners/spawn_slug.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/slug_generator.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

generic_entity *spawn_slug(float x, float y, float angle, phy_vect *init_path, int init_path_length) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_SLUG;
  stored_ge->slug = (slug){ 0 };
  generate_slug(&stored_ge->slug, x, y, angle, init_path, init_path_length);
  return stored_ge;
}

