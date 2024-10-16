#include "scene/scenes/main/environment.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include <stdbool.h>

void generate_environment_element(int tex_id, float x, float y, float angle, bool is_animated) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info cpci = { PHYSICS_BOX, 145.0f, 184.0f, 1.0f, phy_v(x, y), angle, phy_v(0.0f, 0.0f) };
  comp_physics *cp = sys_physics_add(entity, &cpci);
  /*comp_renderer *cr = sys_renderer_add(entity);*/
  phy_body_set_angle(cp->body, angle);
  /*generate_renderer(cr, cp, tex_id, 0);*/
}

