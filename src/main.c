#include "networking/client.h"
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "sender") == 0) {
      sender_run("Room123");
    }
    else if (strcmp(argv[1], "receiver") == 0) {
      receiver_run("Room123");
    }
  }
  return 0;
}

