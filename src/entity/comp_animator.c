#include "entity/comp_animator.h"
#include "data_utl/thread_utl.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_renderer.h"
#include "data_utl/map_utl.h"
#include "khg_ecs/ecs.h"
#include "khg_thd/thread.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id ANIMATOR_COMPONENT_SIGNATURE;
utl_map *ANIMATOR_INFO_MAP = NULL;

void comp_animator_register(comp_animator *ca, ecs_ecs *ecs) {
  ca->id = ecs_register_component(ecs, sizeof(comp_animator), NULL, NULL);
  ANIMATOR_COMPONENT_SIGNATURE = ca->id; 
}

void sys_animator_register(sys_animator *sa, ecs_ecs *ecs) {
  sa->id = ecs_register_system(ecs, sys_animator_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sa->id, ANIMATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sa->id, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sa->id, DESTROYER_COMPONENT_SIGNATURE);
  sa->ecs = *ecs;
  ANIMATOR_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_animator_add(ecs_ecs *ecs, ecs_id *eid, animator_info *info) {
  ecs_add(ecs, *eid, ANIMATOR_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(ANIMATOR_INFO_MAP, eid, info);
}

void sys_animator_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(ANIMATOR_INFO_MAP);
  }
}

void *update_animator_entities(void *arg) {
  thread_data *data = (thread_data *)arg;
  animator_info *info;
  renderer_info *r_info;
  destroyer_info *d_info;
  for (int id = data->start; id < data->end; id++) {
    info = utl_map_at(ANIMATOR_INFO_MAP, &data->entities[id]);
    r_info = utl_map_at(RENDERER_INFO_MAP, &data->entities[id]);
    d_info = utl_map_at(DESTROYER_INFO_MAP, &data->entities[id]);
    if (--info->frame_timer == 0) {
      r_info->tex_id = r_info->tex_id < info->max_tex_id ? r_info->tex_id + 1 : info->min_tex_id;
      info->frame_timer = info->frame_duration;
    }
    if (info->destroy_on_max && r_info->tex_id == info->max_tex_id) {
      d_info->destroy_now = true;
    }
  }
  return NULL;
}

ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  const int thread_count = THREAD_COUNT;
  struct thd_thread threads[thread_count];
  thread_data t_data[thread_count];
  int chunk_size = entity_count / thread_count;
  for (int i = 0; i < thread_count; i++) {
    t_data[i].entities = entities;
    t_data[i].start = i * chunk_size;
    t_data[i].end = (i == thread_count - 1) ? entity_count : t_data[i].start + chunk_size;
    thd_thread_create(&threads[i], NULL, update_animator_entities, &t_data[i], NULL);
  }

  for (int i = 0; i < thread_count; i++) {
    thd_thread_join(threads[i], NULL);
  }
  return 0;
}

