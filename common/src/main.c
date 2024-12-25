#include "game.h"
#include "khg_khs/khs.h"
#include "networking/client.h"
#include "networking/server.h"
#include "networking/test.h"
#include <string.h>

const int main(int argc, char *argv[]) {
  /* khs_load_included_libs();*/
  /*const char *prog = */
  /*  "let sum = 0\n"*/
  /*  "for 1 11 with i do \n"*/
  /*  "  sum += i \n"*/
  /*  "end \n"*/
  /*  "sum";*/
  /*khs_val result = khs_eval(prog, strlen(prog), KHS_PRINT_ERR);*/
  /*double num = khs_as_num(result);*/
  /*printf("%f, %f\n", num, 1.0 + 2.0 + 3.0 + 4.0 + 5.0 + 6.0 + 7.0 + 8.0 + 9.0 + 10.0);*/
  /* return 0;*/
  if (argc > 1) {
    if (strcmp(argv[1], "server") == 0) {
      server_run("KHGSVR_V1");
    }
    else if (strcmp(argv[1], "hoster") == 0) {
      hoster_run();
    }
  }
  else {
    return game_run();
    return main_dbm();
  }
  return 0;
}

