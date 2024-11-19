#include "scene/scenes/main/player_builder.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include "resources/texture_loader.h"
#include "rig/rig.h"

player_info PLAYER_INFO = { 0 };

void build_player(const int min_tex_id, const int max_tex_id, const float x, const float y, const int render_layer) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 300.0f, 256.0f, 1.0f, phy_vector2_new(x, y), 0.0f, true, true };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, min_tex_id, render_layer, 1.0f, false };
  rig_builder rb = { 6, PLAYER_BODY, 3, phy_vector2_new(-6.0f, 27.5f) };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci, &rb);
  add_bone(&cr->rig, phy_vector2_new(1.5f, -32.5f), PLAYER_HEAD, 4, (bone *)utl_array_at(cr->rig.bones, 3));
  add_bone(&cr->rig, phy_vector2_new(17.5f, 5.0f), PLAYER_ARM_L, 2, (bone *)utl_array_at(cr->rig.bones, 3));
  add_bone(&cr->rig, phy_vector2_new(-12.5f, 5.0f), PLAYER_ARM_R, 5, (bone *)utl_array_at(cr->rig.bones, 3));
  add_bone(&cr->rig, phy_vector2_new(-6.0f, 11.5f), PLAYER_LEG_L, 1, (bone *)utl_array_at(cr->rig.bones, 3));
  add_bone(&cr->rig, phy_vector2_new(6.0f, 11.5f), PLAYER_LEG_R, 0, (bone *)utl_array_at(cr->rig.bones, 3));
  comp_animator_constructor_info comp_animator_ci = { min_tex_id, max_tex_id, 0.032f, false };
  comp_animator *ca = sys_animator_add(entity, &comp_animator_ci);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, 250.0f }, phy_vector2_new(0.0f, 0.0f) };
  comp_light *cl = sys_light_add(entity, &comp_light_ci);
  comp_mover_constructor_info comp_mover_ci = { cp->body, 200, 200, PLAYER_IDLE_0, PLAYER_IDLE_0, PLAYER_IDLE_0, PLAYER_IDLE_0 };
  comp_mover *cm = sys_mover_add(entity, &comp_mover_ci);
  PLAYER_INFO = (player_info){ entity, cp, cr, ca, cl, cm };
}

