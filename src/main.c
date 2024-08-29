#include "game.h"
#include "module_test/ipa_test.h"
#include "module_test/tcp_test.h"

int main(int argc, char *argv[]) {
  return tcp_client_send();
  return ipa_test();
  return game_run();
}
