#include "entity/comp_destroyer.h"
#include "controllers/elements/element_destruction_controller.h"
#include "data_utl/thread_utl.h"
#include "entity/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id DESTROYER_COMPONENT_SIGNATURE;
destroyer_info NO_DESTROYER = { 0 };
utl_vector *DESTROYER_INFO = NULL;

void comp_destroyer_register(comp_destroyer *cd, ecs_ecs *ecs) {
  cd->id = ecs_register_component(ecs, sizeof(comp_destroyer), NULL, NULL);
  DESTROYER_COMPONENT_SIGNATURE = cd->id; 
}

void sys_destroyer_register(sys_destroyer *sd, ecs_ecs *ecs) {
  sd->id = ecs_register_system(ecs, sys_destroyer_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sd->id, DESTROYER_COMPONENT_SIGNATURE);
  sd->ecs = *ecs;
  DESTROYER_INFO = utl_vector_create(sizeof(destroyer_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(DESTROYER_INFO, &NO_DESTROYER);
  }
}

void sys_destroyer_add(ecs_ecs *ecs, ecs_id *eid, destroyer_info *info) {
  ecs_add(ecs, *eid, DESTROYER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(DESTROYER_INFO, *eid, info);
}

void sys_destroyer_free(bool need_free) {
  if (need_free) {
    utl_vector_deallocate(DESTROYER_INFO);
  }
}

void *update_destroyer_entities(void *arg) {
  thread_data *data = (thread_data *)arg;

  return NULL;
}

ecs_ret sys_destroyer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  destroyer_info *info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(DESTROYER_INFO, entities[id]);
    //info = utl_map_at(DESTROYER_INFO_MAP, &entities[id]);
    if (info->destroy_now) {
      element_destroy(entities[id]);
    }
  }
  /*
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
  */
  return 0;
}

