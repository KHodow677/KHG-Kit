#include "scene/scenes/main/zone_builder.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_zone.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vector.h"

void build_zone(const phy_rigid_body *player_body, const int min_x, const int max_x) {
  const ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 0.0f, 0.0f, 1.0f, NV_VECTOR2((min_x + max_x) / 2.0f, 0.0f), 0.0f, false, false };
  const comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_zone_constructor_info comp_zone_ci = { player_body, min_x, max_x };
  const comp_zone *cz = sys_zone_add(entity, &comp_zone_ci);
}

