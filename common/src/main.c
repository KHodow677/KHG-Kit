#include "game.h"
#include "network/server.h"

const int main(int argc, char *argv[]) {
  /*return server();*/
  if (argc > 1) {
  }
  else {
    return game_run();
  }
  return 0;
}

