#include "game.h"
#include "module_test/tcp_test.h"

int main(int argc, char *argv[]) {
  return tcp_server_test(argc, argv);
  return game_run();
}
