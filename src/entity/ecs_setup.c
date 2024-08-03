#include "ecs_setup.h"
#include <stdio.h>

ecs_id_t PosComp;
ecs_id_t VelComp;

ecs_id_t System1;

void register_components(ecs_t *ecs) {
  PosComp = ecs_register_component(ecs, sizeof(pos_t), NULL, NULL);
  VelComp = ecs_register_component(ecs, sizeof(vel_t), NULL, NULL);
}

ecs_ret_t system_update(ecs_t *ecs, ecs_id_t *entities, int entity_count, ecs_dt_t dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  for (int id = 0; id < entity_count; id++) {
    printf("%u ", entities[id]);
  }
  printf("\n");
  return 0;
}

void register_systems(ecs_t *ecs) {
  System1 = ecs_register_system(ecs, system_update, NULL, NULL, NULL);

  ecs_require_component(ecs, System1, PosComp);
}


int ecs_setup() {
  ecs_t *ecs = ecs_new(1024, NULL);

  register_components(ecs);
  register_systems(ecs);

  ecs_id_t e1 = ecs_create(ecs);
  ecs_id_t e2 = ecs_create(ecs);
  ecs_id_t e3 = ecs_create(ecs);

  printf("---------------------------------------------------------------\n");
  printf("Created entities: %u, %u, %u\n", e1, e2, e3);
  printf("---------------------------------------------------------------\n");

  printf("PosComp added to: %u\n", e1);
  ecs_add(ecs, e1, PosComp, NULL);

  printf("---------------------------------------------------------------\n");
  printf("PosComp added to: %u\n", e2);
  printf("VeloComp added to: %u\n", e2);

  ecs_add(ecs, e2, PosComp, NULL);
  ecs_add(ecs, e2, VelComp, NULL);

  printf("---------------------------------------------------------------\n");
  printf("PosComp added to: %u\n", e3);
  printf("VeloComp added to: %u\n", e3);
  printf("RectComp added to: %u\n", e3);

  ecs_add(ecs, e3, PosComp, NULL);
  ecs_add(ecs, e3, VelComp, NULL);

  printf("---------------------------------------------------------------\n");

  printf("Executing system 1\n");
  ecs_update_system(ecs, System1, 0.0f);

  printf("---------------------------------------------------------------\n");

  ecs_free(ecs);

  return 0;
}

