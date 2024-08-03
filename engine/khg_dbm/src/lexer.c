#include "khg_dbm/lexer.h"
#include <string.h>
#include <stdlib.h>

char* Keywords[] = {
  "select", "from", "where", "join", "on",
  "table", "create", "delete", "insert", "into",
  "update", "drop", "varchar", "int", "char",
  "str", "set", "drop", "table", "fields"
};

char *Whitespace = "\t \n\r";
char *Operator = "(=,)";
char *Digits = "0123456789";

dbm_token *dbm_create_token(char *value, dbm_token_type type) {
  dbm_token *new_tok = (dbm_token *)malloc(sizeof(dbm_token));
  new_tok->value = malloc(strlen(value) * sizeof(char));
  strcpy(new_tok->value, value);
  new_tok->type = type;
  new_tok->next = NULL;
  return new_tok;
}

void dbm_add_token(dbm_token **parent, dbm_token **child) {
  (*parent)->next = *child;
}

int dbm_is_any(char c, char *in_str) {
  char *idx_ptr = strchr(in_str, c);
  if (idx_ptr) {
    return 1;
  } 
  else {
    return 0;
  }
}

off_t dbm_read_string(char *buf, char **payload, int *val_string) {
  off_t pos = 0;
  if (buf[pos] == '\'') {
    *val_string = 1;
  } 
  else {
    *val_string = 0;
  }
  while (pos < strlen(buf)) {
    pos++;
    if (*val_string) {
        if(buf[pos] == '\'' && buf[pos - 1] != '\\') {
          pos++;
          break;
        }
    } 
    else {
      if(dbm_is_any(buf[pos], Whitespace)) {
        break;
      }
      if(dbm_is_any(buf[pos], Operator)) {
        break;
      }
    }
  }
  *payload = (char *)malloc(pos * sizeof(char) + 1);
  strncpy(*payload, buf, pos);
  return pos;
}

int dbm_is_keyword(char *str){
  for (int i = 0; i < sizeof(Keywords) / sizeof(char *); i++) {
    if (strcasecmp(str, Keywords[i]) == 0){
      return i;
    }
  }
  return -1;
}

int dbm_is_number(char *str){
  if (strspn(str, Digits) == strlen(str)) {
    return 1;
  }
  else {
    return 0;
  }
}

dbm_token *dbm_tokenize(char *query){
  dbm_token *root = NULL;
  dbm_token *it = NULL;
  int is_value;
  char *str_payload;
  for (off_t pos = 0; pos < strlen(query);) {
    if (dbm_is_any(query[pos], Whitespace)) {
      pos++;
    } 
    else {
      dbm_token *new_tok;
      if (!dbm_is_any(*(query + pos), Operator)) {
        pos+=dbm_read_string(query + pos, &str_payload, &is_value);
        if (!is_value) {
          int keyword = dbm_is_keyword(str_payload);
          if (keyword != -1){
            new_tok = dbm_create_token(Keywords[keyword], KEYWORD);
          } 
          else {
            if(dbm_is_number(str_payload)) {
              new_tok = dbm_create_token(str_payload, VALUE);
            } 
            else {
              new_tok = dbm_create_token(str_payload, IDENTIFIER);
            }
          }
        } 
        else {
          new_tok = dbm_create_token(str_payload, VALUE);
        }
      } 
      else {
        str_payload = (char *) malloc(2 * sizeof(char));
        snprintf(str_payload, 2, "%c", *(query + pos));
        new_tok = dbm_create_token(str_payload, OPERATOR);
        pos++;
      }
      if (root == NULL) {
        root = new_tok;
        it = new_tok;
      } 
      else {
        dbm_add_token(&it, &new_tok);
        it = new_tok;
      }
    }
  }
  return root;
}
