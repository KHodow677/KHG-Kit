#include "khg_khs/lexer.h"
#include "khg_khs/util.h"
#include <assert.h>

#define KHS_LEX_STATE_COPY(to, from) (*to = *((lex_state *)(from)))

#define KHS_KEYWORD(str, tok) { str, sizeof(str) - 1, tok }
struct { 
  const char *str; khs_size len; unsigned char tok_type; } keywords[] = {
  KHS_KEYWORD("let", KHS_TOK_LET),
  KHS_KEYWORD("function", KHS_TOK_FN),
  KHS_KEYWORD("with", KHS_TOK_FN),
  KHS_KEYWORD("do", KHS_TOK_DO),
  KHS_KEYWORD("end", KHS_TOK_END),
  KHS_KEYWORD("=", KHS_TOK_ASSIGN),
  KHS_KEYWORD("global", KHS_TOK_GLOBAL),
  KHS_KEYWORD("...", KHS_TOK_VARARGS)
};

#define KHS_ERR_MSG(x) { *str_len = sizeof(x) - 1; return x; }

const char *khs_lex_err_str(khs_lex_token tok, size_t *str_len) {
  assert(tok.type == KHS_TOK_ERR);
  switch (tok.content.err_type) {
    case KHS_TOK_ERR_NULL: 
      KHS_ERR_MSG("Lexing error: Null");
    case KHS_TOK_ERR_MISSING_STR_END: 
      KHS_ERR_MSG("Lexing error: Missing string terminator");
    case KHS_TOK_ERR_SIZE_OVERFLOW: 
      KHS_ERR_MSG( "Lexing error: Symbol too long");
    case KHS_TOK_ERR_INT_OVERFLOW: 
      KHS_ERR_MSG("Lexing error: Integer too large");
    default: 
      KHS_ERR_MSG("Lexing error: None");
  }
}

static bool khs_is_whitespace(char c) {
  switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      return true;
    default:
      return false;
  }
}

static bool khs_is_op_character(char c) {
  switch (c) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '?':
    case '^':
    case '=':
    case '<':
    case '>':
    case '!':
    case ':':
    case '.':
      return true;
    default:
      return false;
  }
}

static unsigned char khs_get_special_char(char c) {
  switch (c) {
    case '(':
      return KHS_TOK_OPEN_BRACKET;
    case ')':
      return KHS_TOK_CLOSE_BRACKET;
    case ';':
      return KHS_TOK_ENDLINE;
    default:
      return KHS_TOK_NULL;
  }
}

static bool khs_is_valid_symbol_char(char c) {
  return !khs_is_whitespace(c) && khs_get_special_char(c) == KHS_TOK_NULL;
}

static bool khs_is_digit(char c) {
  return c >= '0' && c <= '9';
}

static bool khs_is_at_end(khs_lex_state *state) {
  return state->at == state->end;
}

static void khs_skip_whitespace(khs_lex_state *state) {
  bool inside_comment = false;
  while (!khs_is_at_end(state) && (inside_comment || khs_is_whitespace(*state->at) || *state->at == '#')) {
    if (*state->at == '#') {
      inside_comment = !inside_comment;
    }
    else if (*state->at == '\n') {
      inside_comment = false;
    }
    state->at++;
  }
}

static bool khs_eqlstrcmp(const char *a, const char *b, khs_size len) {
  for (; len != 0; len--) {
    if (*a != *b) {
      return false;
    }
    a++;
    b++;
  }
  return true;
}

static unsigned char khs_match_keyword(const char *sym, khs_size len) {
  for (unsigned i = 0; i < KHS_UTIL_LENOF(keywords); i++) {
    if (keywords[i].len == len) {
      if (khs_eqlstrcmp(keywords[i].str, sym, len)) {
        return keywords[i].tok_type;
      }
    }
  }
  return KHS_TOK_NULL;
}

khs_lex_token khs_parse_number(khs_lex_state *state) {
  const char *start = state->at;
  khs_float float_v = 0;
  while (!khs_is_at_end(state) && (khs_is_digit(*state->at) || *state->at == '\'')) {
    if (*state->at == '\'') {
      state->at++;
      continue;
    }
    float_v *= 10;
    float_v += *state->at - '0';
    state->at++;
  }
  if (!khs_is_at_end(state) && *state->at == '.') {
    state->at++;
    khs_float decimals = 0;
    khs_float pow = 1.0;
    while (!khs_is_at_end(state) && (khs_is_digit(*state->at) || *state->at == '\'')) {
      if (*state->at == '\'') {
        state->at++;
        continue;
      }
      pow *= 0.1;
      decimals += (*state->at - '0') * pow;
      state->at++;
    }
    float_v += decimals;
  }
  return (khs_lex_token){ KHS_TOK_NUMBER, start, state->at - start, { .number = float_v } };
}

khs_lex_token khs_next_token(khs_lex_state *state) {
START:
  if(khs_is_at_end(state)) {
    return (khs_lex_token){ .type = KHS_TOK_EOF, .src = state->end, .len = 0 };
  }
  if (*state->at == '\n' || *state->at == ';') {
    const char *start = state->at;
    state->at++;
    khs_skip_whitespace(state);
    return (khs_lex_token){ .type = KHS_TOK_ENDLINE, .src = start, .len = 1 };
  }
  if (khs_is_whitespace(*state->at)) {
    while (!khs_is_at_end(state) && khs_is_whitespace(*state->at) && *state->at != '\n') {
      state->at++;
    }
    goto START;
  }
  if (*state->at == '#') {
    state->at++;
    while (!khs_is_at_end(state)) {
      if (*state->at == '\n') {
        break;
      }
      if (*state->at == '#') {
        state->at++;
        break;
      }
      state->at++;
    }
    goto START;
  }
  unsigned char special_char = khs_get_special_char(*state->at);
  if (special_char != KHS_TOK_NULL) {
    state->at++;
    return (khs_lex_token) { special_char, state->at - 1, 1, { .err_type = 0 } };
  }
  if (*state->at == '"') {
    const char *start = state->at;
    do {
      state->at++;
      if (khs_is_at_end(state))
        return (khs_lex_token) { .type = KHS_TOK_ERR, .src = start, .len = state->at - start, .content = { .err_type = KHS_TOK_ERR_MISSING_STR_END } };
    } while (*state->at != '"');
    state->at++;
    size_t str_len = state->at - start;
    if (str_len > KHS_SIZE_MAX)
      return (khs_lex_token){ KHS_TOK_ERR, start, 1, { .err_type = KHS_TOK_ERR_SIZE_OVERFLOW } };
    return (khs_lex_token){ KHS_TOK_STRING, start, str_len, { .sym = { start + 1, str_len - 2 } } };
  }
  if (*state->at == '-') {
    state->at++;
    if (khs_is_at_end(state) || !khs_is_digit(*state->at)) {
      state->at--;
    }
    else {
      khs_lex_token tok = khs_parse_number(state);
      tok.content.number = -tok.content.number;
      return tok;
    }
  }
  if (khs_is_digit(*state->at)) {
    return khs_parse_number(state);
  }
  const char *sym_start = state->at;
  while (!khs_is_at_end(state) && khs_is_valid_symbol_char(*state->at)) {
    state->at++;
  }
  size_t sym_len = state->at - sym_start;
  if (sym_len > KHS_SIZE_MAX) {
    return (khs_lex_token){ KHS_TOK_ERR, sym_start, 1, { .err_type = KHS_TOK_ERR_SIZE_OVERFLOW } };
  }
  khs_lex_token sym_tok = { KHS_TOK_SYM, sym_start, sym_len, { .sym = { sym_start, sym_len } } };
  if(sym_len > 0) {
    if(sym_start[0] == ':' && sym_len > 1) {
      sym_tok.type = KHS_TOK_STRING;
      sym_tok.content.sym.len--;
      sym_tok.content.sym.str++;
    } 
    else if (khs_is_op_character(sym_start[sym_len - 1])) {
      sym_tok.type = KHS_TOK_OP;
      if(sym_len > 1) {
        if(sym_start[sym_len - 1] == ':')
          sym_tok.content.sym.len--;
        else if(sym_start[sym_len - 1] == '=' && sym_start[0] != '=') {
          sym_tok.type = KHS_TOK_FN_ASSIGN;
          sym_tok.content.sym.len--;
        }
      }
    }
  }
  unsigned char keyword = khs_match_keyword(sym_start, sym_len);
  if (keyword != KHS_TOK_NULL) {
    sym_tok.type = keyword;
  }
  return sym_tok;
}

khs_lex_token khs_lex_peek(khs_lex_state *state) {
  return state->buffer;
}

khs_lex_token khs_lex_pop(khs_lex_state *state) {
  khs_lex_token tok = state->buffer;
  state->buffer = khs_next_token(state);
  return tok;
}

bool khs_lex_accept(khs_lex_state *state, unsigned char type, khs_lex_token *opt_tok) {
  khs_lex_token tok = khs_lex_peek(state);
  if (tok.type == type) {
    khs_lex_pop(state);
    if (opt_tok != NULL) {
      *opt_tok = tok;
      assert(opt_tok->type != KHS_TOK_NULL);
    }
    return true;
  }
  return false;
}

void khs_lex_state_init(khs_lex_state *state, const char *src, size_t src_len) {
  state->src = src;
  state->end = src + src_len;
  state->at = src;
  state->buffer = khs_next_token(state);
}

