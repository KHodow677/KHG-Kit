#include "khg_kin/body.h"
#include "khg_kin/collision.h"
#include "khg_kin/engine.h"
#include <stdlib.h>

kin_engine kin_engine_create() {
  return (kin_engine)calloc(1, sizeof(struct kin_engine_raw));
}

void kin_engine_deallocate(kin_engine* e) {
  while((*e)->bodies) {
    kin_body_deallocate(&((*e)->bodies));
  }
  free(*e);
  *e = 0;
}

void kin_engine_step(kin_engine e, float dt) {
  for(kin_body b = e->bodies; b != 0; b = b->next) {
    kin_body_step(b, dt);
    kin_body_reset(b);
  }
  kin_collision_data collision;
  for(kin_body b1 = e->bodies; b1; b1 = b1->next) {
    for(kin_body b2 = b1->next; b2; b2 = b2->next) {
      for(kin_shape s1 = b1->shapes; s1; s1 = s1->next) {
        for(kin_shape s2 = b2->shapes; s2; s2 = s2->next) {
          if(kin_collide_bb(s1, s2)) {
            kin_collide_shapes(&collision, s1, s2);
          }
        }
      }
    }
  }
}

void kin_engine_body_add(kin_engine e, kin_body b) {
  b->next = e->bodies;
  e->bodies = b;
}

void kin_engine_body_remove(kin_engine e, kin_body b) {
  kin_body* cur = &(e->bodies);
  while((*cur) && (*cur != b)) {
    cur = &((*cur)->next);
  }
  kin_body_deallocate(cur);
}

void kin_engine_print(kin_engine e) {
  for(kin_body b = e->bodies; b; b = b->next) {
    kin_body_print(b);
  }
}
