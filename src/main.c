#include "networking/client.h"
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "hoster") == 0) {
      hoster_run("Room123");
    }
    else if (strcmp(argv[1], "joiner") == 0) {
      joiner_run("Room123");
    }
  }
  return 0;
}

