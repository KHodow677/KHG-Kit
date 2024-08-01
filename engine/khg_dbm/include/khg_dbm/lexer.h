#pragma once

#include <stdio.h>

#ifdef _WINDOWS
#define strcasecmp stricmp
#endif

typedef enum { KEYWORD, IDENTIFIER, VALUE, OPERATOR} TokenType;

typedef struct _Token{
  char *value;
  TokenType type;
  struct _Token *next;
} Token;



Token *create_token(char *value, TokenType type);
void add_token(Token **parent, Token **child);
int is_any(char c, char *in_str);
off_t read_string(char *buf, char **payload, int *val_string);
int is_keyword(char *str);
int is_number(char *str);
Token *tokenize(char *query);
