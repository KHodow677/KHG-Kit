#include "networking/client.h"
#include "networking/server.h"
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "server") == 0) {
      game_server server;
      server_start(&server, "localhost", "3000", 10);
      server_run(&server);
      server_shutdown(&server);
      return 0;
    }
  }
  client_run();
}
