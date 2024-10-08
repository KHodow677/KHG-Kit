#include "networking/client.h"
#include "networking/server.h"
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "hoster") == 0) {
      test_tcp_client_receive();
      /*hoster_run("Room123");*/
    }
    else if (strcmp(argv[1], "joiner") == 0) {
      test_tcp_client_send();
      /*joiner_run("Room123");*/
    }
  }
  server_run();
  return 0;
}

