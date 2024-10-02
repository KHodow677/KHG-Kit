#include "game.h"
#include "net.h"
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "server") == 0) {
      run_server();
      return 0;
    }
    else if (strcmp(argv[1], "client") == 0) {
      run_client();
      return 0;
    }
  }
  run_server_client();
  return 0;
  return game_run();
}

