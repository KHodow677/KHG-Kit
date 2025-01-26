#include "game.h"
#include "util/network/server.h"
#include <string.h>

const int main(int argc, char *argv[]) {
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
