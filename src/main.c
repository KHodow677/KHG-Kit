#include "ecs_test.h"
#include "phy_test.h"
#include "tcp_test.h"
#include "gfx_test.h"

int main(int argc, char *argv[]) {
  ecs_test();
  phy_test();
  tcp_test();
  gfx_start();
  return 0;
}
