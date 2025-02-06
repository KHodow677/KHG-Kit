#define NAMESPACE_KIN_USE

#include "game.h"
#include "util/network/server.h"
#include <string.h>

const int main(int argc, char *argv[]) {
  // kin_engine e = NAMESPACE_KIN()->engine_create();
  // kin_body b = NAMESPACE_KIN()->body_create();
  // NAMESPACE_KIN()->engine_body_add(e, b);
  // NAMESPACE_KIN()->body_force_add(b, (kin_vec){ 0.0f, 1.0f });
  // b->mass = 1.0f;
  // b->inertia = 1.0f;
  // printf("=== t = %f ===\n", 0.0f);
  // NAMESPACE_KIN()->engine_print(e);
  // printf("\n=== t = %f ===\n", 0.1f);
  // NAMESPACE_KIN()->engine_step(e, 0.1f);
  // NAMESPACE_KIN()->engine_print(e);
  // printf("\n=== t = %f ===\n", 0.2f);
  // NAMESPACE_KIN()->engine_step(e, 0.1f);
  // NAMESPACE_KIN()->engine_print(e);
  // NAMESPACE_KIN()->engine_deallocate(&e);
  // return 0;
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
