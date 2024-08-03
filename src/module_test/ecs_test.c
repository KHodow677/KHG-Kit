#include "ecs_test.h"
#include <stdio.h>

ecs_id test_pos_comp;
ecs_id test_vel_comp;
ecs_id test_rect_comp;

ecs_id test_system_1;
ecs_id test_system_2;
ecs_id test_system_3;

void test_register_components(ecs_ecs *ecs) {
  test_pos_comp = ecs_register_component(ecs, sizeof(test_pos_t), NULL, NULL);
  test_vel_comp = ecs_register_component(ecs, sizeof(test_vel_t), NULL, NULL);
  test_rect_comp = ecs_register_component(ecs, sizeof(test_rect_t), NULL, NULL);
}

ecs_ret test_system_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  for (int id = 0; id < entity_count; id++) {
    printf("%u ", entities[id]);
  }
  printf("\n");
  return 0;
}

void test_register_systems(ecs_ecs *ecs) {
  test_system_1 = ecs_register_system(ecs, test_system_update, NULL, NULL, NULL);
  test_system_2 = ecs_register_system(ecs, test_system_update, NULL, NULL, NULL);
  test_system_3 = ecs_register_system(ecs, test_system_update, NULL, NULL, NULL);

  ecs_require_component(ecs, test_system_1, test_pos_comp);
  ecs_require_component(ecs, test_system_2, test_pos_comp);
  ecs_require_component(ecs, test_system_3, test_vel_comp);

  ecs_require_component(ecs, test_system_3, test_pos_comp);
  ecs_require_component(ecs, test_system_3, test_vel_comp);
  ecs_require_component(ecs, test_system_3, test_rect_comp);
}


int ecs_test() {
  ecs_ecs *ecs = ecs_new(1024, NULL);

  test_register_components(ecs);
  test_register_systems(ecs);

  ecs_id e1 = ecs_create(ecs);
  ecs_id e2 = ecs_create(ecs);
  ecs_id e3 = ecs_create(ecs);

  printf("---------------------------------------------------------------\n");
  printf("Created entities: %u, %u, %u\n", e1, e2, e3);
  printf("---------------------------------------------------------------\n");

  printf("PosComp added to: %u\n", e1);
  ecs_add(ecs, e1, test_pos_comp, NULL);

  printf("---------------------------------------------------------------\n");
  printf("PosComp added to: %u\n", e2);
  printf("VeloComp added to: %u\n", e2);

  ecs_add(ecs, e2, test_pos_comp, NULL);
  ecs_add(ecs, e2, test_vel_comp, NULL);

  printf("---------------------------------------------------------------\n");
  printf("PosComp added to: %u\n", e3);
  printf("VeloComp added to: %u\n", e3);
  printf("RectComp added to: %u\n", e3);

  ecs_add(ecs, e3, test_pos_comp, NULL);
  ecs_add(ecs, e3, test_vel_comp, NULL);
  ecs_add(ecs, e3, test_rect_comp, NULL);

  printf("---------------------------------------------------------------\n");

  printf("Executing system 1\n");
  ecs_update_system(ecs, test_system_1, 0.0f);

  printf("Executing system 2\n");
  ecs_update_system(ecs, test_system_2, 0.0f);

  printf("Executing system 3\n");
  ecs_update_system(ecs, test_system_3, 0.0f);

  printf("---------------------------------------------------------------\n");

  ecs_free(ecs);

  return 0;
}

