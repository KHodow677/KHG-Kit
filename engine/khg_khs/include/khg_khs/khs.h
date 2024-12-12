#pragma once

#include <stddef.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

typedef double i_float;
#define BERYL_NUM_MAX_INT (1ll << DBL_MANT_DIG)

typedef unsigned i_size;
#define I_SIZE_MAX UINT_MAX

typedef unsigned short i_refc;
#define I_REFC_MAX USHRT_MAX

typedef unsigned long long beryl_tag;
#define BERYL_MAX_TAGS ULLONG_MAX

typedef unsigned long long large_uint_type;
#define LARGE_UINT_TYPE_MAX ULLONG_MAX

enum {
	TYPE_NULL,
	TYPE_NUMBER,
	TYPE_BOOL,
	TYPE_STR,
	TYPE_TABLE,
	TYPE_ARRAY,
	TYPE_TAG,
	TYPE_FN,
	TYPE_EXT_FN,
	TYPE_OBJECT,
	TYPE_ERR
};

enum beryl_err_action {
	BERYL_PRINT_ERR,
	BERYL_CATCH_ERR,
	BERYL_PROP_ERR
};

#define BERYL_INLINE_STR_MAX_LEN (sizeof(void*))

#define BERYL_NULL ((i_val) { .type = TYPE_NULL } )
#define BERYL_NUMBER(f) ((i_val) { .type = TYPE_NUMBER, .val.num_v = f } )
#define BERYL_BOOL(b) ((i_val) { .type = TYPE_BOOL, .val.bool_v = b } )
#define BERYL_TRUE BERYL_BOOL(true)
#define BERYL_FALSE BERYL_BOOL(false)
#define BERYL_STATIC_STR(s, l) ((i_val) { .type = TYPE_STR, .len = l, .val.str = s, .managed = false })
#define BERYL_CONST_STR(s) BERYL_STATIC_STR((s), sizeof(s) - 1)
#define BERYL_STATIC_ARRAY(a, l) ((i_val) { .type = TYPE_ARRAY, .len = l, .val.static_array = a, .managed = false })
#define BERYL_ERR(msg_str) ((i_val) { .type = TYPE_ERR, .managed = false, .len = sizeof(msg_str) - 1, .val.str = msg_str } ) 
#define BERYL_EXT_FN(fn_ptr) ((i_val) { .type = TYPE_EXT_FN, .val.ext_fn = fn_ptr } )
#define BERYL_TYPEOF(v) ((v).type)
#define BERYL_LENOF(v) ((v).len)
#define BERYL_MAKE_LITERAL_FN(args, body) (args " do " body)
#define BERYL_LITERAL_FN(args, ...) ((i_val) { .type = TYPE_FN, .managed = false, .len = sizeof(BERYL_MAKE_LITERAL_FN(#args, #__VA_ARGS__)) - 1, .val.fn = BERYL_MAKE_LITERAL_FN(#args, #__VA_ARGS__) })
#define BERYL_STATIC_TABLE_SIZE(l) (sizeof(beryl_table) + sizeof(i_val_pair) * ((l)*3 / 2))

typedef struct beryl_table beryl_table;
typedef struct i_val i_val;
typedef struct i_managed_array i_managed_array;
typedef struct i_managed_str i_managed_str;
typedef struct beryl_external_fn beryl_external_fn;
typedef struct beryl_fn beryl_fn;
typedef struct beryl_object beryl_object;

struct i_val {
	union {
		i_managed_str *managed_str;
		beryl_table *table;
		const char *str;
		char inline_str[BERYL_INLINE_STR_MAX_LEN];
		i_float num_v;
		bool bool_v;
		beryl_external_fn *ext_fn;
		const char *fn;
		beryl_tag tag;
		i_val *static_array;
		i_managed_array *managed_array;
		beryl_object *object;
	} val;
	i_size len;
	bool managed;
	unsigned char type;
};

struct beryl_external_fn {
	int arity;
	bool auto_release;
	const char *name;
	size_t name_len;
	i_val (*fn)(const i_val *, i_size);
};

typedef struct i_val_pair {
	i_val key, val;
} i_val_pair;

struct beryl_table {
	i_size cap;
	i_refc ref_c;
	i_val_pair entries[];
};

typedef struct beryl_object_class {
	void (*free)(beryl_object *);
	i_val (*call)(beryl_object *, const i_val *, i_size);
	size_t obj_size;
	const char *name;
	size_t name_len;
} beryl_object_class;

struct beryl_object {
	beryl_object_class *obj_class;
	i_refc ref_c;
};

const char *beryl_get_raw_str(const i_val *str);
i_float beryl_as_num(i_val val);
bool beryl_as_bool(i_val val);
beryl_tag beryl_as_tag(i_val val);

i_val beryl_new_tag();

bool beryl_is_integer(i_val val);

int beryl_val_cmp(i_val a, i_val b);

bool beryl_set_var(const char *name, i_size name_len, i_val val, bool as_const);

void *beryl_new_scope();
void beryl_restore_scope(void *prev);
bool beryl_bind_name(const char *name, i_size name_len, i_val val, bool is_const);

i_val beryl_new_table(i_size cap, bool padding);

i_val beryl_new_array(i_size len, const i_val *items, i_size fit_for, bool padded);
const i_val *beryl_get_raw_array(i_val array);

i_size beryl_get_array_capacity(i_val array);

bool beryl_array_push(i_val *array, i_val val);

i_val beryl_static_table(i_size cap, unsigned char *bytes, size_t bytes_size);

i_val_pair *beryl_iter_table(i_val table_v, i_val_pair *iter);

int beryl_table_insert(i_val *table_v, i_val key, i_val val, bool replace);
bool beryl_table_should_grow(i_val table, i_size extra);

void beryl_set_io(void (*print)(void *, const char *, size_t), void (*print_i_val)(void *, i_val), void *err_f);
void beryl_print_i_val(void *f, i_val val);
void beryl_i_vals_printf(void *f, const char *str, size_t strlen, const i_val *vals, unsigned n); // N must be at max 10

void beryl_set_mem(void *(*alloc)(size_t), void (*free)(void *), void *(*realloc)(void *, size_t));

i_val beryl_new_string(i_size len, const char *from);

i_val beryl_new_object(beryl_object_class *obj_class);
beryl_object_class *beryl_object_class_type(i_val val);
beryl_object *beryl_as_object(i_val val);

i_val beryl_str_as_err(i_val str);
i_val beryl_err_as_str(i_val str);

i_val beryl_retain(i_val val);
void beryl_retain_values(const i_val *items, size_t n);
void beryl_release(i_val val);
void beryl_release_values(const i_val *items, size_t n);

i_refc beryl_get_refcount(i_val val);

void beryl_blame_arg(i_val val);

void beryl_free(void *ptr);
void *beryl_alloc(size_t n);
void *beryl_realloc(void *ptr, size_t n);

void *beryl_talloc(size_t n);
void beryl_tfree(void *ptr);

i_val beryl_call(i_val fn, const i_val *args, size_t n_args, bool borrow);
i_val beryl_pcall(i_val fn, const i_val *args, size_t n_args, bool borrow, bool print_trace);

i_val beryl_eval(const char *src, size_t src_len, enum beryl_err_action err);

void beryl_clear();
bool beryl_load_included_libs();

