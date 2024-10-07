#include "networking/client.h"
#include "networking/server.h"
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "server") == 0) {
      return server_start();
    }
  }
  tcp_client_receive();
}

