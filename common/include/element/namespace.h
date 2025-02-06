#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "util/light.h"
#include "util/ovr_tile.h"

typedef struct element_ecs_info {
  ecs_id component_signature;
  ecs_id system_signature;
  void *init_info;
} element_ecs_info;

typedef struct comp_light {
  ecs_id id;
  light light;
  phy_vector2 offset;
} comp_light;

typedef struct comp_physics {
  ecs_id id;
  phy_rigid_body *body;
  phy_shape *shape;
  phy_vector2 vel;
  bool is_moving;
  phy_vector2 init_pos;
  phy_vector2 init_size;
  float init_ang;
} comp_physics;

typedef struct comp_render {
  ecs_id id;
  unsigned int tex_id_loc;
  ovr_tile_info ovr_tile;
  unsigned int render_layer;
  float parallax_value;
  bool flipped;
} comp_render;

typedef struct comp_tile {
  ecs_id id;
  bool loaded;
  ovr_tile_info tile;
} comp_tile;

#if defined(NAMESPACE_ELEMENT_IMPL) || defined(NAMESPACE_ELEMENT_USE)
typedef struct element_namespace {
  void (*ecs_setup)(void);
  void (*ecs_cleanup)(void);
  void (*comp_light_register)(void);
  void (*sys_light_register)(void);
  comp_light *(*sys_light_add)(const ecs_id, comp_light *);
  void (*comp_physics_register)(void);
  void (*sys_physics_register)(void);
  comp_physics *(*sys_physics_add)(const ecs_id, comp_physics *);
  void (*comp_render_register)(void);
  void (*sys_render_register)(void);
  comp_render *(*sys_render_add)(const ecs_id, comp_render *);
  void (*comp_tile_register)(void);
  void (*sys_tile_register)(void);
  comp_tile *(*sys_tile_add)(const ecs_id, comp_tile *);
  unsigned int ENTITY_COUNT;
  ecs_ecs *ECS;
  element_ecs_info LIGHT_INFO;
  element_ecs_info PHYSICS_INFO;
  element_ecs_info RENDER_INFO;
  element_ecs_info TILE_INFO;
} element_namespace;
#endif

#ifdef NAMESPACE_ELEMENT_IMPL
extern element_namespace NAMESPACE_ELEMENT_INTERNAL;
#endif

#ifdef NAMESPACE_ELEMENT_USE
element_namespace *NAMESPACE_ELEMENT(void);
#endif

