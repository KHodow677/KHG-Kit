#include "entity/comp_physics.h"
#include "data_utl/kinematic_utl.h"
#include "data_utl/thread_utl.h"
#include "entity/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_thd/thread.h"
#include "khg_utl/vector.h"
#include <math.h>
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
physics_info NO_PHYSICS = { 0 };
utl_vector *PHYSICS_INFO = NULL;

void comp_physics_register(comp_physics *cp, ecs_ecs *ecs) {
  cp->id = ecs_register_component(ecs, sizeof(comp_physics), NULL, NULL);
  PHYSICS_COMPONENT_SIGNATURE = cp->id; 
}

void sys_physics_register(sys_physics *sp, ecs_ecs *ecs) {
  sp->id = ecs_register_system(ecs, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sp->id, PHYSICS_COMPONENT_SIGNATURE);
  sp->ecs = *ecs;
  PHYSICS_INFO = utl_vector_create(sizeof(physics_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(PHYSICS_INFO, &NO_PHYSICS);
  }
}

void sys_physics_add(ecs_ecs *ecs, ecs_id *eid, physics_info *info) {
  ecs_add(ecs, *eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(PHYSICS_INFO, *eid, info);
}

void sys_physics_free(bool need_free) {
  if (need_free) {
    utl_vector_deallocate(PHYSICS_INFO);
  }
}

void *update_physics_entities(void *arg) {
  thread_data *data = (thread_data *)arg;
  physics_info *info;
  for (int id = data->start; id < data->end; id++) {
    info = utl_vector_at(PHYSICS_INFO, data->entities[id]);
    float current_ang = normalize_angle(phy_body_get_angle(info->body));
    phy_body_set_velocity(info->body, cpv(sinf(current_ang)*info->target_vel, -cosf(current_ang)*info->target_vel));
    phy_body_set_angular_velocity(info->body, info->target_ang_vel);
  }
  return NULL;
}

ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
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
    thd_thread_create(&threads[i], NULL, update_physics_entities, &t_data[i], NULL);
  }

  for (int i = 0; i < thread_count; i++) {
    thd_thread_join(threads[i], NULL);
  }
  return 0;
}

