#pragma once

typedef struct _wrx_state {
  char op;
	short s[2];
	union {
    char c;
		char *bv;
		short idx;
	} data;
} wrx_state;

typedef struct _wregex_t {
	wrx_state *states;
	short n_states;
	short ns;
	short	start, stop;
	int	n_subm;
	char *p;
} wregex_t;

typedef struct _wregmatch_t {
	const char *beg;
	const char *end;
} wregmatch_t;

wregex_t *wrx_comp(const char *pattern, int *e, int *ep);
int wrx_exec(const wregex_t *wreg, const char *str, wregmatch_t subm[], int nsm);
void wrx_free(wregex_t *wreg);
const char *wrx_error(int code);
