#pragma once

typedef enum dbm_i {
  int_t,
	str_t,
	char_t,
	null_t
} dbm_i;

typedef struct dbm_generic {
	dbm_i type;
	union {
		int int_v;
		char *str_v;
		char char_v;
	} value;
} dbm_generic;

extern int dbm_is_type(dbm_generic **data, dbm_i type);
extern int dbm_verify_types(char *types);
extern char dbm_format_type(dbm_generic **data);
extern dbm_i dbm_get_type(char type);

int dbm_geneq(dbm_generic *gen_1, dbm_generic *gen_2);

extern void dbm_set_int(dbm_generic **g, void *value);
extern int dbm_get_int(dbm_generic **g);

extern void dbm_set_char(dbm_generic **g, void *value);
extern char dbm_get_char(dbm_generic **g);

extern void dbm_set_str(dbm_generic **g, void *value);
extern char *dbm_get_str(dbm_generic **g);

extern void dbm_create_generic(dbm_generic **new_gen, dbm_i type, void *value);
extern void dbm_create_def_generic(dbm_generic **new_gen, dbm_i type);
extern void dbm_create_generic_str(dbm_generic **new_gen, dbm_i type, char *initial_cond);

