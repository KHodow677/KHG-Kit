#include "scene/scenes/main/player_builder.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "resources/anim_loader.h"
#include "resources/area_loader.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "rig/anim.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"

player_info PLAYER_INFO = { 0 };

void build_player(const float x, const float y, const unsigned int render_layer) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 50.0f, 140.0f, 5.0f, phy_vector2_new(x, y), 0.0f, true, true, false };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, cp->shape, PLAYER_BODY, PLAYER_RIG, NO_AREA_ID, render_layer, 1.0f, false, false };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
  generate_animation_from_path(&cr->rig, "res/assets/data/anim/frames/player/idle/", "player_frame", 0, 8);
  set_state_and_frame(&cr->rig, 0, 0);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, 50.0f }, phy_vector2_new(0.0f, 0.0f) };
  comp_light *cl = sys_light_add(entity, &comp_light_ci);
  comp_animator_constructor_info comp_animator_ci = { 0, 1, 0.128f, false };
  comp_animator *ca = sys_animator_add(entity, &comp_animator_ci);
  comp_mover_constructor_info comp_mover_ci = { cp->body, 300, 0.256 };
  comp_mover *cm = sys_mover_add(entity, &comp_mover_ci);
  PLAYER_INFO = (player_info){ entity, cp, cr, cl, ca, cm };
}

