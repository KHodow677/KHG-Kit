#pragma once

#include <stddef.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

typedef double khs_float;
#define KHS_NUM_MAX_INT (1ll << DBL_MANT_DIG)

typedef unsigned khs_size;
#define KHS_SIZE_MAX UINT_MAX

typedef unsigned short khs_refc;
#define KHS_REFC_MAX USHRT_MAX

typedef unsigned long long khs_tag;
#define KHS_MAX_TAGS ULLONG_MAX

typedef unsigned long long khs_large_uint_type;
#define KHS_LARGE_UINT_TYPE_MAX ULLONG_MAX

enum {
  KHS_TYPE_NULL,
  KHS_TYPE_NUMBER,
  KHS_TYPE_BOOL,
  KHS_TYPE_STR,
  KHS_TYPE_TABLE,
  KHS_TYPE_ARRAY,
  KHS_TYPE_TAG,
  KHS_TYPE_FN,
  KHS_TYPE_EXT_FN,
  KHS_TYPE_OBJECT,
  KHS_TYPE_ERR
};

typedef enum khs_err_action {
  KHS_PRINT_ERR,
  KHS_CATCH_ERR,
  KHS_PROP_ERR
} khs_err_action;

#define KHS_INLINE_STR_MAX_LEN (sizeof(void *))

#define KHS_NULL ((khs_val){ .type = KHS_TYPE_NULL })
#define KHS_NUMBER(f) ((khs_val){ .type = KHS_TYPE_NUMBER, .val.num_v = f })
#define KHS_BOOL(b) ((khs_val){ .type = KHS_TYPE_BOOL, .val.bool_v = b })
#define KHS_TRUE KHS_BOOL(true)
#define KHS_FALSE KHS_BOOL(false)
#define KHS_STATIC_STR(s, l) ((khs_val){ .type = KHS_TYPE_STR, .len = l, .val.str = s, .managed = false })
#define KHS_CONST_STR(s) KHS_STATIC_STR((s), sizeof(s) - 1)
#define KHS_STATIC_ARRAY(a, l) ((khs_val){ .type = KHS_TYPE_ARRAY, .len = l, .val.static_array = a, .managed = false })
#define KHS_ERR(msg_str) ((khs_val){ .type = KHS_TYPE_ERR, .managed = false, .len = sizeof(msg_str) - 1, .val.str = msg_str } ) 
#define KHS_EXT_FN(fn_ptr) ((khs_val) { .type = KHS_TYPE_EXT_FN, .val.ext_fn = fn_ptr } )
#define KHS_TYPEOF(v) ((v).type)
#define KHS_LENOF(v) ((v).len)
#define KHS_MAKE_LITERAL_FN(args, body) (args " do " body)
#define KHS_LITERAL_FN(args, ...) ((khs_val) { .type = KHS_TYPE_FN, .managed = false, .len = sizeof(KHS_MAKE_LITERAL_FN(#args, #__VA_ARGS__)) - 1, .val.fn = KHS_MAKE_LITERAL_FN(#args, #__VA_ARGS__) })
#define KHS_STATIC_TABLE_SIZE(l) (sizeof(khs_table) + sizeof(i_val_pair) * ((l)*3 / 2))

typedef struct khs_table khs_table;
typedef struct khs_val khs_val;
typedef struct khs_managed_array khs_managed_array;
typedef struct khs_managed_str khs_managed_str;
typedef struct khs_external_fn khs_external_fn;
typedef struct khs_object khs_object;

struct khs_val {
  union {
    khs_managed_str *managed_str;
    khs_table *table;
    const char *str;
    char inline_str[KHS_INLINE_STR_MAX_LEN];
    khs_float num_v;
    bool bool_v;
    khs_external_fn *ext_fn;
    const char *fn;
    khs_tag tag;
    khs_val *static_array;
    khs_managed_array *managed_array;
    khs_object *object;
  } val;
  khs_size len;
  bool managed;
  unsigned char type;
};

struct khs_external_fn {
  int arity;
  bool auto_release;
  const char *name;
  size_t name_len;
  khs_val (*fn)(const khs_val *, khs_size);
};

typedef struct khs_val_pair {
  khs_val key, val;
} khs_val_pair;

struct khs_table {
  khs_size cap;
  khs_refc ref_c;
  khs_val_pair entries[];
};

typedef struct beryl_object_class {
  void (*free)(khs_object *);
  khs_val (*call)(khs_object *, const khs_val *, khs_size);
  size_t obj_size;
  const char *name;
  size_t name_len;
} beryl_object_class;

struct khs_object {
  beryl_object_class *obj_class;
  khs_refc ref_c;
};

const char *khs_get_raw_str(const khs_val *str);
khs_float khs_as_num(khs_val val);
bool khs_as_bool(khs_val val);
khs_tag khs_as_tag(khs_val val);

khs_val khs_new_tag();

bool khs_is_integer(khs_val val);

int khs_val_cmp(khs_val a, khs_val b);

bool khs_set_var(const char *name, khs_size name_len, khs_val val, bool as_const);

void *khs_new_scope();
void khs_restore_scope(void *prev);
bool khs_bind_name(const char *name, khs_size name_len, khs_val val, bool is_const);

khs_val khs_new_table(khs_size cap, bool padding);

khs_val khs_new_array(khs_size len, const khs_val *items, khs_size fit_for, bool padded);
const khs_val *khs_get_raw_array(khs_val array);

khs_size khs_get_array_capacity(khs_val array);

bool khs_array_push(khs_val *array, khs_val val);

khs_val khs_static_table(khs_size cap, unsigned char *bytes, size_t bytes_size);

khs_val_pair *khs_iter_table(khs_val table_v, khs_val_pair *iter);

int khs_table_insert(khs_val *table_v, khs_val key, khs_val val, bool replace);
bool khs_table_should_grow(khs_val table, khs_size extra);

void khs_set_io(void (*print)(void *, const char *, size_t), void (*print_i_val)(void *, khs_val), void *err_f);
void khs_print_val(void *f, khs_val val);
void khs_vals_printf(void *f, const char *str, size_t strlen, const khs_val *vals, unsigned n); // N must be at max 10

void khs_set_mem(void *(*alloc)(size_t), void (*free)(void *), void *(*realloc)(void *, size_t));

khs_val khs_new_string(khs_size len, const char *from);

khs_val khs_new_object(beryl_object_class *obj_class);
beryl_object_class *khs_object_class_type(khs_val val);
khs_object *khs_as_object(khs_val val);

khs_val khs_str_as_err(khs_val str);
khs_val khs_err_as_str(khs_val str);

khs_val khs_retain(khs_val val);
void khs_retain_values(const khs_val *items, size_t n);
void khs_release(khs_val val);
void khs_release_values(const khs_val *items, size_t n);

khs_refc khs_get_refcount(khs_val val);

void khs_blame_arg(khs_val val);

void khs_free(void *ptr);
void *khs_alloc(size_t n);
void *khs_realloc(void *ptr, size_t n);

void *khs_talloc(size_t n);
void khs_tfree(void *ptr);

khs_val khs_call(khs_val fn, const khs_val *args, size_t n_args, bool borrow);
khs_val khs_pcall(khs_val fn, const khs_val *args, size_t n_args, bool borrow, bool print_trace);

khs_val khs_eval(const char *src, size_t src_len, enum khs_err_action err);

void khs_clear();
bool khs_load_included_libs();

