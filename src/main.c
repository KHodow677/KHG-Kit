#include "game.h"
#include "generators/components/map_generator.h"
#include "module_test/tcp_test.h"
#include <string.h>

int main(int argc, char *argv[]) {
  return load_map("Map");
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

