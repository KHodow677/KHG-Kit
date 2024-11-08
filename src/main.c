#include "game.h"
#include "networking/client.h"
#include "networking/server.h"
#include "networking/test.h"
#include "skeleton/skeleton_test.h"
#include <string.h>

const int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "hoster") == 0) {
      hoster_run();
    }
    else if (strcmp(argv[1], "joiner") == 0) {
      joiner_run();
    }
  }
  else {
    return skeleton_test();
    return game_run();
    return main_dbm();
    server_run();
  }
  return 0;
}

