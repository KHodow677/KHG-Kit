#include "game.h"
#include "khg_kin/body.h"
#include "khg_kin/engine.h"
#include "khg_kin/namespace.h"
#include "util/network/server.h"
#include <stdio.h>
#include <string.h>

const int main(int argc, char *argv[]) {
  kin_engine e = kin_engine_create();
  kin_body b = kin_body_create();
  kin_engine_body_add(e, b);
  kin_body_force_add(b, (kin_vec){ 0.0f, 1.0f });
  b->mass = 1.0f;
  b->inertia = 1.0f;
  printf("=== t = %f ===\n", 0.0f);
  kin_engine_print(e);
  printf("\n=== t = %f ===\n", 0.1f);
  kin_engine_step(e, 0.1f);
  kin_engine_print(e);
  printf("\n=== t = %f ===\n", 0.2f);
  kin_engine_step(e, 0.1f);
  kin_engine_print(e);
  kin_engine_deallocate(&e);
  return 0;
  if (argc > 1) {
    if (!strcmp(argv[1], "server")) {
      return server();
    }
  }
  else {
    return game_run();
  }
  return 0;
}
