#include "game.h"
#include "game_manager.h"
#include "generators/components/map_generator.h"
#include "khg_utl/vector.h"
#include "module_test/tcp_test.h"
#include <string.h>

int main(int argc, char *argv[]) {
  load_map("Map");
  for (int i = 0; i < utl_vector_size(GAME_MAP); i++) {
    utl_vector **row =  utl_vector_at(GAME_MAP, i);
    utl_vector_deallocate(*row);
  }
  utl_vector_deallocate(GAME_MAP);
  if (argc > 1) {
    if (strcmp(argv[1], "send") == 0) {
      return tcp_client_send();
    }
    else if (strcmp(argv[1], "receive") == 0) {
      return tcp_client_receive();
    }
  }
  return game_run();
}

