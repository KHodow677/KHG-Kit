#include "game.h"
#include <stdio.h>

const int main(int argc, char *argv[]) {
  printf("KHG Kit\n");
  if (argc > 1) {
  }
  else {
    return game_run();
  }
  return 0;
}
