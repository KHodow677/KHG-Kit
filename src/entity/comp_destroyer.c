#include "entity/comp_destroyer.h"
#include "controllers/elements/element_destruction_controller.h"
#include "data_utl/map_utl.h"
#include "data_utl/thread_utl.h"
#include "khg_ecs/ecs.h"
#include "khg_thd/thread.h"
#include "khg_utl/map.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id DESTROYER_COMPONENT_SIGNATURE;
utl_map *DESTROYER_INFO_MAP = NULL;

void comp_destroyer_register(comp_destroyer *cd, ecs_ecs *ecs) {
  cd->id = ecs_register_component(ecs, sizeof(comp_destroyer), NULL, NULL);
  DESTROYER_COMPONENT_SIGNATURE = cd->id; 
}

void sys_destroyer_register(sys_destroyer *sd, ecs_ecs *ecs) {
  sd->id = ecs_register_system(ecs, sys_destroyer_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sd->id, DESTROYER_COMPONENT_SIGNATURE);
  sd->ecs = *ecs;
  DESTROYER_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_destroyer_add(ecs_ecs *ecs, ecs_id *eid, destroyer_info *info) {
  ecs_add(ecs, *eid, DESTROYER_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(DESTROYER_INFO_MAP, eid, info);
}

void sys_destroyer_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(DESTROYER_INFO_MAP);
  }
}

void *update_destroyer_entities(void *arg) {
  thread_data *data = (thread_data *)arg;
  destroyer_info *info;
  for (int id = data->start; id < data->end; id++) {
    info = utl_map_at(DESTROYER_INFO_MAP, &data->entities[id]);
    if (info->destroy_now) {
      element_destroy(data->entities[id]);
    }
  }
  return NULL;
}

ecs_ret sys_destroyer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
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
    thd_thread_create(&threads[i], NULL, update_destroyer_entities, &t_data[i], NULL);
  }
  for (int i = 0; i < thread_count; i++) {
    thd_thread_join(threads[i], NULL);
  }
  return 0;
}

