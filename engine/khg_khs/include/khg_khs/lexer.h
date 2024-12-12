#pragma once

#include "khg_khs/khs.h"
#include <stdbool.h>

enum {
	TOK_NULL,
	TOK_LET,
	TOK_GLOBAL,
	TOK_VARARGS,
	TOK_DO,
	TOK_END,
	TOK_FN,
	TOK_SYM,
	TOK_STRING,
	TOK_OP,
	TOK_ASSIGN,
	TOK_ENDLINE,
	TOK_NUMBER,
	TOK_OPEN_BRACKET,
	TOK_CLOSE_BRACKET,
	TOK_FN_ASSIGN,
	TOK_ERR,
	TOK_EOF
};

enum {
	TOK_ERR_OK,
	TOK_ERR_NULL,
	TOK_ERR_MISSING_STR_END,
	TOK_ERR_SIZE_OVERFLOW,
	TOK_ERR_INT_OVERFLOW
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
		khs_float number;
		int err_type;
	} content;
} lex_token;

typedef struct lex_state {
	const char *src, *at, *end;
	lex_token buffer;
} lex_state;

const char *lex_err_str(lex_token tok, size_t *str_len);

void lex_state_init(lex_state *state, const char *src, size_t src_len);

lex_token lex_peek(lex_state *state);
lex_token lex_pop(lex_state *state);

bool lex_accept(lex_state *state, unsigned char type, lex_token *opt_tok);

