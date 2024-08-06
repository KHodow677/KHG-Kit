#pragma once

#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#ifndef strcasecmp
#define strcasecmp stricmp
#endif
#endif

typedef enum { 
  KEYWORD, 
  IDENTIFIER, 
  VALUE, 
  OPERATOR
} dbm_token_type;

typedef struct dbm_token {
  char *value;
  dbm_token_type type;
  struct dbm_token *next;
} dbm_token;

dbm_token *dbm_create_token(char *value, dbm_token_type type);
void dbm_add_token(dbm_token **parent, dbm_token **child);
int dbm_is_any(char c, char *in_str);
off_t dbm_read_string(char *buf, char **payload, int *val_string);
int dbm_is_keyword(char *str);
int dbm_is_number(char *str);
dbm_token *dbm_tokenize(char *query);

