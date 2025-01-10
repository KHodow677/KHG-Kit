#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"

void build_ovr_map(const phy_rigid_body *player_body, phy_vector2 pos) {
  const ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 0.0f, 0.0f, 1.0f, pos, 0.0f, false, false, false };
  const comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { 0 };
  const comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
}

