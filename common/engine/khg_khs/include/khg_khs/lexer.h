#pragma once

#include "khg_khs/khs.h"
#include <stdbool.h>

enum {
  KHS_TOK_NULL,
  KHS_TOK_LET,
  KHS_TOK_GLOBAL,
  KHS_TOK_VARARGS,
  KHS_TOK_DO,
  KHS_TOK_END,
  KHS_TOK_FN,
  KHS_TOK_SYM,
  KHS_TOK_STRING,
  KHS_TOK_OP,
  KHS_TOK_ASSIGN,
  KHS_TOK_ENDLINE,
  KHS_TOK_NUMBER,
  KHS_TOK_OPEN_BRACKET,
  KHS_TOK_CLOSE_BRACKET,
  KHS_TOK_FN_ASSIGN,
  KHS_TOK_ERR,
  KHS_TOK_EOF
};

enum {
  KHS_TOK_ERR_OK,
  KHS_TOK_ERR_NULL,
  KHS_TOK_ERR_MISSING_STR_END,
  KHS_TOK_ERR_SIZE_OVERFLOW,
  KHS_TOK_ERR_INT_OVERFLOW
};

typedef struct lex_token {
  unsigned char type;
  const char *src;
  khs_size len;
  union {
    struct {
      const char *str;
      khs_size len;
    } sym;
    float number;
    int err_type;
  } content;
} khs_lex_token;

typedef struct lex_state {
  const char *src, *at, *end;
  khs_lex_token buffer;
} khs_lex_state;

const char *khs_lex_err_str(khs_lex_token tok, unsigned int *str_len);

void khs_lex_state_init(khs_lex_state *state, const char *src, unsigned int src_len);

khs_lex_token khs_lex_peek(khs_lex_state *state);
khs_lex_token khs_lex_pop(khs_lex_state *state);

bool khs_lex_accept(khs_lex_state *state, unsigned char type, khs_lex_token *opt_tok);

