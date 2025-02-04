#define NAMESPACE_KIN_IMPL

#include "khg_kin/namespace.h"
#include <stdlib.h>

kin_engine engine_create() {
  return (kin_engine)calloc(1, sizeof(struct kin_engine_raw));
}

void engine_deallocate(kin_engine* engine) {
  while((*engine)->bodies) {
    NAMESPACE_KIN_INTERNAL.body_deallocate(&((*engine)->bodies));
  }
  free(*engine);
  *engine = 0;
}

void engine_step(kin_engine engine, float dt) {
  for(kin_body b = engine->bodies; b != 0; b = b->next) {
    NAMESPACE_KIN_INTERNAL.body_step(b, dt);
    NAMESPACE_KIN_INTERNAL.body_reset(b);
  }
  kin_collision_data collision;
  for(kin_body b1 = engine->bodies; b1; b1 = b1->next) {
    for(kin_body b2 = b1->next; b2; b2 = b2->next) {
      for(kin_shape s1 = b1->shapes; s1; s1 = s1->next) {
        for(kin_shape s2 = b2->shapes; s2; s2 = s2->next) {
          if(NAMESPACE_KIN_INTERNAL.collide_bb(s1, s2)) {
            NAMESPACE_KIN_INTERNAL.collide_shapes(&collision, s1, s2);
          }
        }
      }
    }
  }
}

void engine_body_add(kin_engine engine, kin_body body) {
  body->next = engine->bodies;
  engine->bodies = body;
}

void engine_body_remove(kin_engine engine, kin_body body) {
  kin_body* cur = &(engine->bodies);
  while((*cur) && (*cur != body)) {
    cur = &((*cur)->next);
  }
  NAMESPACE_KIN_INTERNAL.body_deallocate(cur);
}

void engine_print(kin_engine engine) {
  for(kin_body b = engine->bodies; b; b = b->next) {
    NAMESPACE_KIN_INTERNAL.body_print(b);
  }
}
