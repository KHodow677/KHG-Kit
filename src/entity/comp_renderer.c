#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_thd/thread.h"
#include "khg_utl/vector.h"
#include <stdlib.h>
#include <stdio.h>

ecs_id RENDERER_COMPONENT_SIGNATURE;
renderer_info NO_RENDERER = { 0 };
utl_vector *RENDERER_INFO = NULL;

typedef struct render_data {
  thd_mutex mutex;
  thd_condition next_layer;
  int threads_working;
  int current_layer;
  ecs_id *elements;
  int element_count;
} render_data;

static void render_pass(render_data* data) {
  renderer_info *info;
  thd_mutex_lock(&data->mutex);
  while (data->current_layer <= 10) {
    while (!data->threads_working) {
      thd_condition_wait(&data->next_layer, &data->mutex);
    }
    for (int id = 0; id < data->element_count; id++) {
      info = utl_vector_at(RENDERER_INFO, data->elements[id]);
      if (data->current_layer != info->render_layer) {
        continue;
      }
      phy_vect pos = phy_body_get_position(info->body);
      phy_vect offset = phy_body_get_center_of_gravity(info->body);
      float angle = phy_body_get_angle(info->body);
      gfx_texture *tex = utl_vector_at(TEXTURE_LOOKUP, info->tex_id);
      tex->angle = angle;
      gfx_image_no_block(pos.x, pos.y, *tex, offset.x, offset.y, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
    }
    data->threads_working--;
    if (!data->threads_working) {
      data->current_layer++;
      data->threads_working = THREAD_COUNT;
      thd_condition_signal(&data->next_layer);
    }
  }
  thd_mutex_unlock(&data->mutex);
}

static ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  render_data r_data;
  r_data.threads_working = THREAD_COUNT;
  r_data.current_layer = 0;
  r_data.elements = entities;
  r_data.element_count = entity_count;
  thd_mutex_init(&r_data.mutex);
  thd_condition_init(&r_data.next_layer);
  for(size_t i = 0; i < THREAD_COUNT; i++) {
    thd_thread_detach(WORKER_THREADS+i, (thd_thread_method)render_pass, &r_data);
  }
  for(size_t i = 0; i < THREAD_COUNT; i++) {
    thd_thread_join(WORKER_THREADS+i);
  }
  thd_condition_destroy(&r_data.next_layer);
  thd_mutex_destroy(&r_data.mutex);
  return 0;
}

void comp_renderer_register(comp_renderer *cr) {
  cr->id = ecs_register_component(ECS, sizeof(comp_renderer), NULL, NULL);
  RENDERER_COMPONENT_SIGNATURE = cr->id; 
}

void sys_renderer_register(sys_renderer *sr) {
  sr->id = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sr->id, PHYSICS_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
  RENDERER_INFO = utl_vector_create(sizeof(renderer_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(RENDERER_INFO, &NO_RENDERER);
  }
}

void sys_renderer_add(ecs_id *eid, renderer_info *info) {
  ecs_add(ECS, *eid, RENDERER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(RENDERER_INFO, *eid, info);
}

void sys_renderer_free(bool need_free) {
  if (need_free) {
    utl_vector_deallocate(RENDERER_INFO);
  }
}

