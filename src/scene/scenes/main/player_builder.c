#include "scene/scenes/main/player_builder.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "rig/anim.h"
#include "rig/rig.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"

player_info PLAYER_INFO = { 0 };

void build_player(const int min_tex_id, const int max_tex_id, const float x, const float y, const int render_layer) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 300.0f, 256.0f, 1.0f, phy_vector2_new(x, y), 0.0f, true, true };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, min_tex_id, render_layer, 1.0f, false };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci, generate_rig_builder_from_file("res/assets/anim/rigs/player.ini", "player_root", 1));
  generate_rig_from_file(&cr->rig, "res/assets/anim/rigs/player.ini", "player_bones");
  generate_animation_from_path(&cr->rig, "res/assets/anim/frames/player/idle/", "player_frame", 0, 8);
  set_state_and_frame(&cr->rig, 0, 0);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, 250.0f }, phy_vector2_new(0.0f, 0.0f) };
  comp_light *cl = sys_light_add(entity, &comp_light_ci);
  comp_animator_constructor_info comp_animator_ci = { 0, 1, 0.128, false };
  comp_animator *ca = sys_animator_add(entity, &comp_animator_ci);
  comp_mover_constructor_info comp_mover_ci = { cp->body, 200, 200 };
  comp_mover *cm = sys_mover_add(entity, &comp_mover_ci);
  PLAYER_INFO = (player_info){ entity, cp, cr, cl, ca, cm };
}

