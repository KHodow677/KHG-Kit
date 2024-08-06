#include "khg_dbm/generics.h"
#include "khg_dbm/util.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

static int def_i = 0;
static char def_c = 'a';
static char* def_s = "";

int dbm_is_type(dbm_generic **data, dbm_i type) {
  if ((*data)->type == type) {
		return true;
	} 
  else {
		return false;
	}
}

int dbm_verify_types(char *types) {
	int result = true;
	for (int i = 0; i < strlen(types); i++) {
		if (types[i] != 'c' && types[i] != 's' && types[i] != 'i'){
			result = false;
		}
	}
	return result;
}

int dbm_geneq(dbm_generic *gen_1, dbm_generic *gen_2) {
  if (gen_1->type == gen_2->type) {
    switch (gen_1->type){
      case int_t:
        return gen_1->value.int_v == gen_2->value.int_v;
      case char_t:
        return gen_1->value.char_v == gen_2->value.char_v;
      case str_t:
        return streq(gen_1->value.str_v, gen_2->value.str_v);
      default:
        return 0;
    }
  }
  return 0;
}

char dbm_format_type(dbm_generic **data) {
  if (dbm_is_type(data, int_t)) {
		return 'i';
	}
  else if (dbm_is_type(data, str_t)) {
		return 's';
	}
  else if (dbm_is_type(data, char_t)) {
		return 'c';
	} 
  else {
		return '\0';
	}
}

dbm_i dbm_get_type(char type) {
	if (type == 'i') {
		return int_t;
	}
	else if (type == 's') {
		return str_t;
	}
	else if (type == 'c') {
		return char_t;
	}
	return null_t;
}

void dbm_set_int(dbm_generic **g, void *value) {
	int *nval = (int *) value;
	(*g)->value.int_v = *nval;
}

int dbm_get_int(dbm_generic **g) {
	return (*g)->value.int_v;
}

void dbm_set_char(dbm_generic **g, void *value) {
	char *nval = (char *) value;
	(*g)->value.char_v = *nval;
}

char dbm_get_char(dbm_generic **g) {
	return (*g)->value.char_v;
}

void dbm_set_str(dbm_generic **g, void *value) {
	char* str_val = (char *) value;
	(*g)->value.str_v = malloc(sizeof(char) * strlen(str_val));
	strcpy((*g)->value.str_v, str_val);
}

char* dbm_get_str(dbm_generic **g) {
	return (*g)->value.str_v;
}

void dbm_create_generic(dbm_generic **new_gen, dbm_i type, void *value) {
	(*new_gen) = (dbm_generic *)malloc(sizeof(dbm_generic));
	(*new_gen)->type = type;
	switch (type) {
		case int_t:
			dbm_set_int(new_gen, value);
			break;
		case char_t:
			dbm_set_char(new_gen, value);
			break;
		case str_t:
			dbm_set_str(new_gen, value);
			break;
		default:
			break;
	}
}

void dbm_create_def_generic(dbm_generic **new_gen, dbm_i type) {
	switch (type) {
		case int_t:
			dbm_create_generic(new_gen, type, &def_i);
			break;
		case char_t:
			dbm_create_generic(new_gen, type, &def_c);
			break;
		case str_t:
			dbm_create_generic(new_gen, type, def_s);
			break;
		default:
			break;
	}
}

void dbm_create_generic_str(dbm_generic **new_gen, dbm_i type, char *initial_cond) {
  int i_val;
  switch (type) {
    case int_t:
      i_val = atoi(initial_cond);
      dbm_create_generic(new_gen, type, &i_val);
      break;
    case char_t:
      dbm_create_generic(new_gen, type, initial_cond + 1);
      break;
    case str_t:
      dbm_create_generic(new_gen, type, initial_cond);
      break;
    default:
      break;
  }
}

