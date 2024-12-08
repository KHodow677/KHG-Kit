#include "game.h"
#include "khg_rgx/regex.h"
#include "networking/client.h"
#include "networking/server.h"
#include "networking/test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int match(const char *p, const char *s) {
	int e, ep;
	wregex_t *r;
	r = wrx_comp(p, &e, &ep);
	if (!r) {
    printf("Compile Error\n");
		exit(EXIT_FAILURE);
	}
  wregmatch_t *subm = calloc(sizeof(wregmatch_t), r->n_subm);
	e = wrx_exec(r, s, subm, r->n_subm);
  printf("%i\n", e);
  if (e == 1) {
    int found_length = subm->end - subm->beg + 1;
    char substring[found_length];
    strncpy(substring, subm->beg, found_length);
    substring[found_length-1] = '\0';
    printf("%s\n", substring);
  }
	free(subm);
	wrx_free(r);
	return e;
}

const int main(int argc, char *argv[]) {
  match("[\\d]{3}", "123");
  return 0;
  if (argc > 1) {
    if (strcmp(argv[1], "hoster") == 0) {
      hoster_run();
    }
    else if (strcmp(argv[1], "joiner") == 0) {
      joiner_run();
    }
  }
  else {
    return game_run();
    return main_dbm();
    server_run();
  }
  return 0;
}

