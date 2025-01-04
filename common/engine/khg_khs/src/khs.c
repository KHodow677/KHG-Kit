#include "khg_khs/khs.h"
#include "khg_khs/lexer.h"
#include "khg_khs/lib/lib.h"
#include <assert.h>

#define KHS_MIN(a, b) ((a) < (b) ? (a) : (b))

#define KHS_ERR_STACK_TRACE_SIZE 16
#define KHS_MAX_BLAMED_ARGS 8
#define KHS_TMP_ALLOC_BUFFER_SIZE 512
#define KHS_STATIC_STACK_SIZE 256
#define KHS_GLOBALS_LOOKUP_TABLE_SIZE 256
#define KHS_EXPR_ARG_STACK_SIZE 128
#define KHS_MAX_EXPR_RECURSION 128

typedef struct khs_managed_str {
  khs_refc ref_c;
  char str[];
} khs_managed_str;

typedef struct khs_managed_array {
  khs_refc ref_c;
  khs_size cap;
  khs_val items[];
} khs_managed_array;

typedef struct khs_stack_trace_entry {
  unsigned char type;
  const char *src, *src_end;
  const char *str;
  unsigned int len;
} khs_stack_trace_entry;

typedef struct khs_scope_namespace {
  const char *start, *end;
} khs_scope_namespace;

typedef struct khs_stack_entry {
  const char *name;
  khs_size name_len;
  khs_val val;
  bool is_const;
  khs_scope_namespace namespace;
} khs_stack_entry;

static unsigned int N_ERR_STACK_TRACE_ENTRIES = 0;
static khs_stack_trace_entry ERR_STACK_TRACE[KHS_ERR_STACK_TRACE_SIZE];

static unsigned int N_BLAMED_ARGS = 0;
static khs_val BLAMED_ARGS[KHS_MAX_BLAMED_ARGS];

static void *KHS_ERRF = NULL;

static void (*PRINT_CALLBACK)(void *, const char *str, unsigned int len) = NULL;
static void (*PRINT_VAL_CALLBACK)(void *, khs_val val) = NULL;

static unsigned char TMP_ALLOC_BUFFER[KHS_TMP_ALLOC_BUFFER_SIZE];
static bool TMP_BUFFER_FREE = true;

static khs_stack_entry STATIC_STACK[KHS_STATIC_STACK_SIZE];
static khs_stack_entry *STACK_BASE = STATIC_STACK, *stack_top = STATIC_STACK, *stack_start = STATIC_STACK;
static khs_stack_entry *STACK_END = STATIC_STACK + KHS_STATIC_STACK_SIZE;

static khs_scope_namespace CURRENT_NAMESPACE = { NULL, NULL };
static khs_stack_entry GLOBALS_LOOKUP[KHS_GLOBALS_LOOKUP_TABLE_SIZE];

static khs_val ARG_STACK[KHS_EXPR_ARG_STACK_SIZE];
static khs_val *ARG_STACK_TOP = ARG_STACK;
static khs_val *ARG_STACK_END = ARG_STACK + KHS_EXPR_ARG_STACK_SIZE;

static bool khs_push_stack_trace(khs_stack_trace_entry entry) {
  if (N_ERR_STACK_TRACE_ENTRIES == KHS_ERR_STACK_TRACE_SIZE) {
    return false;
  }
  ERR_STACK_TRACE[N_ERR_STACK_TRACE_ENTRIES++] = entry;
  return true;
}

static void khs_blame_token(khs_lex_state *lex, khs_lex_token tok) {
  khs_push_stack_trace((khs_stack_trace_entry){ 0, lex->src, lex->end, tok.src, tok.len });
}

static void khs_blame_range(khs_lex_state *lex, const char *start, const char *end) {
  khs_push_stack_trace((khs_stack_trace_entry){ 0, lex->src, lex->end, start, end - start });
}

static void khs_blame_name(const char *str, unsigned int len) {
  khs_push_stack_trace((khs_stack_trace_entry){ .type = 1, .str = str, .len = len });
}

void khs_blame_arg(khs_val val) {
  if (N_BLAMED_ARGS == KHS_MAX_BLAMED_ARGS) {
    return;
  }
  BLAMED_ARGS[N_BLAMED_ARGS++] = khs_retain(val);
}

static void khs_clear_err() {
  for (unsigned int i = 0; i < N_BLAMED_ARGS; i++) {
    khs_release(BLAMED_ARGS[i]);
  }
  N_BLAMED_ARGS = 0;
  N_ERR_STACK_TRACE_ENTRIES = 0;
}

void khs_set_io(void (*print)(void *, const char *, unsigned int), void (*print_i_val)(void *, khs_val), void *err_f) {
  KHS_ERRF = err_f;
  PRINT_CALLBACK = print;
  PRINT_VAL_CALLBACK = print_i_val;
}

void khs_print_val(void *f, khs_val val) {
  if (PRINT_VAL_CALLBACK == NULL) {
    return;
  }
  PRINT_VAL_CALLBACK(f, val);
}

static void khs_print_string(void *f, const char *msg) {
  if (PRINT_CALLBACK == NULL) {
    return;
  }
  unsigned int len = 0;
  for (const char *c = msg; *c != '\0'; c++) {
    len++;
  }
  PRINT_CALLBACK(f, msg, len);
}

static void khs_print_bytes(void *f, const char *b, unsigned int len) {
  if (PRINT_CALLBACK == NULL) {
    return;
  }
  PRINT_CALLBACK(f, b, len);
}

void khs_vals_printf(void *f, const char *str, unsigned int strlen, const khs_val *vals, unsigned n) {
  assert(n <= 10);
  const char *str_end = str + strlen;
  const char *prev = NULL;
  for (const char *c = str; c < str_end; c++) {
    if (*c == '%' && c != str_end - 1) {
      if (prev != NULL) {
        khs_print_bytes(f, prev, c - prev);
        prev = NULL;
      }
      c++;
      if (*c < '0' || *c > '9') {
        continue;
      }
      unsigned i = *c - '0';
      if (i < n) {
        khs_print_val(f, vals[i]);
      }
    } 
    else if (prev == NULL) {
      prev = c;
    }
  }
  if (prev != NULL) {
    khs_print_bytes(f, prev, str_end - prev);
  }
}

static void khs_log_err(khs_val err_val) {
  khs_print_string(KHS_ERRF, "\n----------------\n");
  for (unsigned int i = 1; i <= N_ERR_STACK_TRACE_ENTRIES; i++) {
    unsigned int index = N_ERR_STACK_TRACE_ENTRIES - i;
    khs_stack_trace_entry entry = ERR_STACK_TRACE[index];
    if (entry.type == 1) {
      khs_print_string(KHS_ERRF, "In:\n");
      khs_print_bytes(KHS_ERRF, entry.str, entry.len);
    } 
    else if (entry.type == 0) {
      khs_print_string(KHS_ERRF, "At:\n");
      const char *line_start;
      unsigned int n_tabs = 0;
      unsigned int n_spaces = 0;
      for (line_start = entry.str - 1; line_start >= entry.src; line_start--) {
        if (*line_start == '\n') {
          line_start++;
          break;
        }
        if (*line_start == '\t') {
          n_tabs++;
        }
        else {
          n_spaces++;
        }
      }
      const char *line_end;
      for (line_end = entry.str + entry.len; line_end < entry.src_end; line_end++) {
        if (*line_end == '\n') {
          break;
        }
      }
      if (line_start < entry.src) {
        line_start = entry.src;
      }
      if (line_end < line_start) {
        line_end = line_start;
      }
      khs_print_bytes(KHS_ERRF, line_start, line_end - line_start);
      khs_print_string(KHS_ERRF, "\n");
      while (n_tabs--) {
        khs_print_bytes(KHS_ERRF, "\t", 1);
      }
      while (n_spaces--) {
        khs_print_bytes(KHS_ERRF, " ", 1);
      }
      for (unsigned int i = 0; i < entry.len; i++) {
        khs_print_bytes(KHS_ERRF, "^", 1);
      }
    }
    khs_print_string(KHS_ERRF, "\n----------------\n");
  }
  if (N_BLAMED_ARGS > 0) {
    for (unsigned int i = 0; i < N_BLAMED_ARGS; i++) {
      khs_print_val(KHS_ERRF, BLAMED_ARGS[i]);
      khs_print_string(KHS_ERRF, " ");
    }
    khs_print_string(KHS_ERRF, "\n----------------\n");
  }
  assert(KHS_TYPEOF(err_val) == KHS_TYPE_ERR);
  const char *err_str = khs_get_raw_str(&err_val);
  khs_size err_str_len = KHS_LENOF(err_val);
  khs_print_string(KHS_ERRF, "Error: ");
  khs_vals_printf(KHS_ERRF, err_str, err_str_len, BLAMED_ARGS, N_BLAMED_ARGS);
  khs_print_string(KHS_ERRF, "\n");
}

bool khs_cmp_len_strs(const char *a, unsigned int alen, const char *b, unsigned int blen) {
  if (alen != blen) {
    return false;
  }
  while (alen--) {
    if (*a != *b) {
      return false;
    }
    a++, b++;
  }
  return true;
}

void (*free_callback)(void *ptr) = NULL;
void *(*alloc_callback)(unsigned int n) = NULL;
void *(*realloc_callback)(void *ptr, unsigned int) = NULL;

void khs_set_mem(void *(*alloc)(unsigned int), void (*free)(void *), void *(*realloc)(void *, unsigned int)) {
  free_callback = free;
  alloc_callback = alloc;
  realloc_callback = realloc;
}

void khs_free(void *ptr) {
  if (free_callback == NULL) {
    return;
  }
  free_callback(ptr);
}

void *khs_alloc(unsigned int n) {
  if (alloc_callback == NULL) {
    return NULL;
  }
  return alloc_callback(n);
}

void *khs_realloc(void *ptr, unsigned int n) {
  if (ptr == TMP_ALLOC_BUFFER) {
    assert(!TMP_BUFFER_FREE);
    if (n <= KHS_TMP_ALLOC_BUFFER_SIZE) {
      return ptr;
    }
    unsigned char *new_alloc = khs_alloc(n);
    if (new_alloc == NULL) {
      return NULL;
    }
    unsigned char *p = ptr;
    unsigned int cpy_len = KHS_MIN(KHS_TMP_ALLOC_BUFFER_SIZE, n);
    unsigned char *t = new_alloc;
    while (cpy_len--) {
      *(t++) = *(p++);
    }
    TMP_BUFFER_FREE = true;
    return new_alloc;
  }
  if (realloc_callback == NULL) {
    return NULL;
  }
  return realloc_callback(ptr, n);
}

void *khs_talloc(unsigned int n) {
  if (n < sizeof(TMP_ALLOC_BUFFER) && TMP_BUFFER_FREE) {
    TMP_BUFFER_FREE = false;
    return TMP_ALLOC_BUFFER;
  }
  return khs_alloc(n);
}

void khs_tfree(void *ptr) {
  if (ptr == TMP_ALLOC_BUFFER) {
    assert(!TMP_BUFFER_FREE);
    TMP_BUFFER_FREE = true;
  } 
  else {
    khs_free(ptr);
  }
}

static khs_refc *khs_get_reference_counter(khs_val of_val) {
  switch (of_val.type) {
    case KHS_TYPE_TABLE:
      if (!of_val.managed) {
        return NULL;
      }
      return &of_val.val.table->ref_c;
    case KHS_TYPE_ERR:
    case KHS_TYPE_STR:
      if (!of_val.managed || of_val.len <= KHS_INLINE_STR_MAX_LEN) {
        return NULL;
      }
      return &of_val.val.managed_str->ref_c;
    case KHS_TYPE_ARRAY:
      if (!of_val.managed) {
        return NULL;
      }
      return &of_val.val.managed_array->ref_c;
    case KHS_TYPE_OBJECT:
      return &of_val.val.object->ref_c;
    default:
      return NULL;
  }
}

khs_val khs_retain(khs_val val) {
  khs_refc *ref_counter = khs_get_reference_counter(val);
  if (ref_counter == NULL) {
    return val;
  }
  if (*ref_counter != KHS_REFC_MAX) {
    (*ref_counter)++;
  }
  return val;
}

void khs_retain_values(const khs_val *items, unsigned int n) {
  while (n--) {
    khs_retain(*(items++));
  }
}

void khs_release(khs_val val) {
  khs_refc *ref_counter = khs_get_reference_counter(val);
  if (ref_counter == NULL) {
    return;
  }
  if (*ref_counter == KHS_REFC_MAX) {
    return;
  }
  assert(*ref_counter != 0);
  if (--(*ref_counter) == 0) {
    switch (val.type) {
      case KHS_TYPE_ERR:
      case KHS_TYPE_STR:
        khs_free(val.val.managed_str);
        break;
      case KHS_TYPE_TABLE: {
        khs_val_pair *iter = NULL;
        while( (iter = khs_iter_table(val, iter)) ) {
          khs_release(iter->key);
          khs_release(iter->val);
        }
        khs_free(val.val.table); } 
        break;
      case KHS_TYPE_ARRAY: {
        const khs_val *items = khs_get_raw_array(val);
        for (khs_size i = 0; i < KHS_LENOF(val); i++) {
          khs_release(items[i]);
        }
        khs_free(val.val.managed_array); } 
        break;
      case KHS_TYPE_OBJECT: {
        khs_object *obj = val.val.object;
        if (obj->obj_class->free != NULL) {
          obj->obj_class->free(obj);
        }
        khs_free(obj); } break;
      default:
        assert(false);
    }
  }
}

void khs_release_values(const khs_val *items, unsigned int n) {
  while (n--) {
    khs_release(*(items++));
  }
}

khs_refc khs_get_refcount(khs_val val) {
  khs_refc *counter = khs_get_reference_counter(val);
  if (counter == NULL) {
    return KHS_REFC_MAX;
  }
  return *counter;
}

const char *khs_get_raw_str(const khs_val *str) {
  assert(str->type == KHS_TYPE_STR || str->type == KHS_TYPE_ERR);
  if (!str->managed) {
    return str->val.str;
  }
  if (str->len <= KHS_INLINE_STR_MAX_LEN) {
    return &str->val.inline_str[0];
  }
  return &str->val.managed_str->str[0];
}

khs_float khs_as_num(khs_val val) {
  assert(KHS_TYPEOF(val) == KHS_TYPE_NUMBER);
  return val.val.num_v;
}

bool khs_as_bool(khs_val val) {
  assert(KHS_TYPEOF(val) == KHS_TYPE_BOOL);
  return val.val.bool_v;
}

khs_tag khs_as_tag(khs_val val) {
  assert(KHS_TYPEOF(val) == KHS_TYPE_TAG);
  return val.val.tag;
}

khs_val khs_new_tag() {
  static khs_tag tag_counter = 0;
  if (tag_counter == KHS_MAX_TAGS) {
    return KHS_NULL;
  }
  return (khs_val){ .type = KHS_TYPE_TAG, .len = 0, .managed = false, .val.tag = tag_counter++ };
}

static bool khs_namespace_overlap(khs_scope_namespace a, khs_scope_namespace b) {
  return (a.start >= b.start && a.end <= b.end) || (b.start >= a.start && b.end <= a.end);
}

static bool khs_push_stack(khs_stack_entry *entry) {
  if (stack_top < STACK_END) {
    *stack_top = *entry;
    stack_top++;
    khs_retain(entry->val);
    return true;
  }
  return false;
}

bool khs_bind_name(const char *name, khs_size name_len, khs_val val, bool is_const) {
  khs_stack_entry entry = { name, name_len, val, is_const, { NULL, NULL } };
  return khs_push_stack(&entry);
}

static khs_stack_entry *khs_enter_scope() {
  khs_stack_entry *old_base = STACK_BASE;
  STACK_BASE = stack_top;
  return old_base;
}

void *khs_new_scope() {
  return khs_enter_scope();
}

static void khs_leave_scope(khs_stack_entry *prev_base) {
  for (khs_stack_entry *p = stack_top - 1; p >= STACK_BASE; p--) {
    khs_release(p->val);
  }
  stack_top = STACK_BASE;
  STACK_BASE = prev_base;
}

void khs_restore_scope(void *prev) {
  khs_leave_scope(prev);
}

static khs_stack_entry *khs_get_local(const char *name, khs_size len) {
  for (khs_stack_entry *var = stack_top - 1; var >= STACK_BASE; var--) {
    if (khs_cmp_len_strs(var->name, var->name_len, name, len)) {
      return var;
    }
  }
  return NULL;
}

static khs_stack_entry *khs_index_globals(const char *name, khs_size len) {
  unsigned int hash = 0;
  for (const char *c = name; c < name + len; c++) {
    hash *= 7;
    hash += *c;
  }
  unsigned int index = hash % KHS_GLOBALS_LOOKUP_TABLE_SIZE;
  for (unsigned int i = index; i < KHS_GLOBALS_LOOKUP_TABLE_SIZE; i++) {
    khs_stack_entry *entry = &GLOBALS_LOOKUP[i];
    if (entry->name == NULL) {
      return entry;
    }
    if (khs_cmp_len_strs(entry->name, entry->name_len, name, len)) {
      return entry;
    }
  }
  for (unsigned int i = 0; i < index; i++) {
    khs_stack_entry *entry = &GLOBALS_LOOKUP[i];
    if (entry->name == NULL) {
      return entry;
    }
    if (khs_cmp_len_strs(entry->name, entry->name_len, name, len)) {
      return entry;
    }
  }
  return NULL;
}

static khs_stack_entry *get_global(const char *name, khs_size len) {
  for (khs_stack_entry *var = stack_top - 1; var >= stack_start; var--) {
    if ((var->namespace.start == NULL || khs_namespace_overlap(var->namespace, CURRENT_NAMESPACE)) && khs_cmp_len_strs(var->name, var->name_len, name, len)) {
      return var;
    }
  }
  khs_stack_entry *global = khs_index_globals(name, len);
  if (global == NULL || global->name == NULL) {
    return NULL;
  }
  return global;
}

bool khs_set_var(const char *name, khs_size name_len, khs_val val, bool as_const) {
  khs_stack_entry new_var = { name, name_len, val, as_const, CURRENT_NAMESPACE };
  khs_stack_entry *entry = khs_index_globals(name, name_len);
  if (entry == NULL) {
    return false;
  }
  if (entry->name == NULL) {
    *entry = new_var;
    khs_retain(entry->val);
    return true;
  }
  if (entry->is_const) {
    return false;
  }
  khs_release(entry->val);
  entry->val = khs_retain(val);
  entry->is_const = as_const;
  return true;
}

static bool khs_push_arg(khs_val val) {
  if (ARG_STACK_TOP < ARG_STACK_END) {
    *ARG_STACK_TOP = val;
    ARG_STACK_TOP++;
    return true;
  } 
  else {
    return false;
  }
}

static khs_val *khs_save_arg_state() {
  return ARG_STACK_TOP;
}

static void khs_restore_arg_state(khs_val *state, bool release) {
  if (release) {
    for (khs_val *p = ARG_STACK_TOP - 1; p >= state; p--) {
      khs_release(*p);
    }
  }
  ARG_STACK_TOP = state;
}

bool khs_is_integer(khs_val val) {
  if (KHS_TYPEOF(val) != KHS_TYPE_NUMBER) {
    return false;
  }
  khs_float f = khs_as_num(val);
  if (f >= KHS_NUM_MAX_INT || f <= -KHS_NUM_MAX_INT) {
    return true;
  }
  return (khs_float)(long long)f - f == 0.0;
}

int khs_val_cmp(khs_val a, khs_val b) {
  if (KHS_TYPEOF(a) != KHS_TYPEOF(b)) {
    return 2;
  }
  switch (KHS_TYPEOF(a)) {
    case KHS_TYPE_NULL:
      return 0;
    case KHS_TYPE_BOOL: {
      if (khs_as_bool(a) == khs_as_bool(b)) {
        return 0;
      }
      return 2; }
    case KHS_TYPE_TAG: {
      if (khs_as_tag(a) == khs_as_tag(b)) {
        return 0;
      }
      else {
        return 2;
      } }
    case KHS_TYPE_NUMBER: {
      khs_float n_a = khs_as_num(a);
      khs_float n_b = khs_as_num(b);
      if (n_a == n_b) {
        return 0;
      }
      if (n_a > n_b) {
        return -1;
      }
      return 1; }
    case KHS_TYPE_STR: {
      const char *s_a = khs_get_raw_str(&a);
      const char *s_b = khs_get_raw_str(&b);
      khs_size l_a = KHS_LENOF(a), l_b = KHS_LENOF(b);
      khs_size minl = KHS_MIN(l_a, l_b);
      for (khs_size i = 0; i < minl; i++) {
        if (s_a[i] != s_b[i]) {
          if (s_a[i] > s_b[i]) {
            return -1;
          }
          return 1;
        }
      }
      if (l_a == l_b) {
        return 0;
      }
      if (l_a > l_b) {
        return -1;
      }
      return 1; }
    case KHS_TYPE_ARRAY: {
      const khs_val *a_a = khs_get_raw_array(a);
      const khs_val *a_b = khs_get_raw_array(b);
      khs_size l_a = KHS_LENOF(a), l_b = KHS_LENOF(b);
      khs_size minl = KHS_MIN(l_a, l_b);
      for (khs_size i = 0; i < minl; i++) {
        int cmp = khs_val_cmp(a_a[i], a_b[i]);
        if (cmp == -1 || cmp == 1) {
          return cmp;
        }
        if (cmp == 2) {
          return 2;
        }
      }
      if (l_a == l_b) {
        return 0;
      }
      if (l_a > l_b) {
        return -1;
      }
      return 1; }
    default:
      return 2;
  }
}

khs_val khs_new_string(khs_size len, const char *from) {
  khs_val res = { .type = KHS_TYPE_STR, .managed = true, .len = len };
  char *str;
  if (len <= KHS_INLINE_STR_MAX_LEN) {
    str = &res.val.inline_str[0];
  }
  else {
    khs_managed_str *mstr = khs_alloc(sizeof(khs_managed_str) + sizeof(char) * len);
    if (mstr == NULL) {
      return KHS_NULL;
    }
    mstr->ref_c = 1;
    str = &mstr->str[0];
    res.val.managed_str = mstr;
  }
  if (from != NULL) {
    while (len--) {
      *(str++) = *(from++);
    }
  }
  return res;
}

khs_val khs_new_object(khs_object_class *obj_class) {
  assert(obj_class->obj_size >= sizeof(khs_object));
  khs_object *obj = khs_alloc(obj_class->obj_size);
  if (obj == NULL) {
    return KHS_NULL;
  }
  obj->ref_c = 1;
  obj->obj_class = obj_class;
  khs_val val = { .val = { .object = obj }, .len = 0, .managed = true, .type = KHS_TYPE_OBJECT };
  return val;
}

khs_object_class *khs_object_class_type(khs_val val) {
  if (KHS_TYPEOF(val) != KHS_TYPE_OBJECT) {
    return NULL;
  }
  return khs_as_object(val)->obj_class;
}

khs_object *khs_as_object(khs_val val) {
  assert(KHS_TYPEOF(val) == KHS_TYPE_OBJECT);
  return val.val.object;
}

khs_val khs_str_as_err(khs_val str) {
  assert(KHS_TYPEOF(str) == KHS_TYPE_STR);
  khs_val err = khs_retain(str);
  err.type = KHS_TYPE_ERR;
  return err;
}

khs_val khs_err_as_str(khs_val err) {
  assert(KHS_TYPEOF(err) == KHS_TYPE_ERR);
  khs_val str = khs_retain(err);
  str.type = KHS_TYPE_STR;
  return str;
}

static bool khs_is_hashable(khs_val key) {
  switch(KHS_TYPEOF(key)) {
    case KHS_TYPE_STR:
    case KHS_TYPE_TAG:
    case KHS_TYPE_BOOL:
      return true;
    case KHS_TYPE_NUMBER:
      return khs_is_integer(key);
    default:
      return false;
  }
}

static unsigned int khs_hash_val(khs_val key) {
  assert(khs_is_hashable(key));
  switch(KHS_TYPEOF(key)) {
    case KHS_TYPE_NUMBER:
      return khs_as_num(key);
    case KHS_TYPE_BOOL:
      return khs_as_bool(key);
    case KHS_TYPE_STR: {
      unsigned int hash = 0;
      khs_size len = key.len;
      const char *str = khs_get_raw_str(&key);
      while (len--) {
        hash *= 7;
        hash += *str;
        str++;
      }
      return hash; }
    default:
      assert(false);
      return 0;
  }
}

static khs_val_pair *khs_search_table(khs_table *table, khs_val key, khs_size from, khs_size until) {
  assert(until <= table->cap);
  for (khs_size i = from; i < until; i++) {
    if (KHS_TYPEOF(table->entries[i].key) == KHS_TYPE_NULL) {
      return &table->entries[i];
    }
    if (khs_val_cmp(table->entries[i].key, key) == 0) {
      return &table->entries[i];
    }
  }
  return NULL;
}

static khs_val_pair *table_get(khs_table *table, khs_val key) {
  if (!khs_is_hashable(key)) {
    return NULL;
  }
  if (table->cap == 0) {
    return NULL;
  }
  unsigned int hash = khs_hash_val(key);
  khs_size index = hash % table->cap;
  khs_val_pair *entry = khs_search_table(table, key, index, table->cap);
  if (entry != NULL) {
    return entry;
  }
  return khs_search_table(table, key, 0, index);
}

static khs_val index_table(khs_val table, khs_val key) {
  assert(KHS_TYPEOF(table) == KHS_TYPE_TABLE);
  khs_val_pair *entry = table_get(table.val.table, key);
  if (entry == NULL || KHS_TYPEOF(entry->key) == KHS_TYPE_NULL) {
    return KHS_NULL;
  }
  return khs_retain(entry->val);
}

int khs_table_insert(khs_val *table_v, khs_val key, khs_val val, bool replace) {
  assert(KHS_TYPEOF(*table_v) == KHS_TYPE_TABLE);
  khs_table *table = table_v->val.table;
  assert(table->ref_c == 1);
  assert(table_v->managed);
  if (!khs_is_hashable(key)) {
    return 3;
  }
  if (table_v->len == table->cap) {
    return 1;
  }
  assert(table->cap > table_v->len);
  khs_val_pair *entry = table_get(table, key);
  assert(entry != NULL);
  if (KHS_TYPEOF(entry->key) != KHS_TYPE_NULL) {
    if (!replace) {
      return 2;
    }
    khs_release(entry->val);
    khs_release(entry->key);
  } 
  else {
    table_v->len++;
  }
  entry->key = khs_retain(key);
  entry->val = khs_retain(val);
  return 0;  
}

khs_val khs_new_table(khs_size cap, bool padding) {
  if (padding) {
    khs_size padded_size = (cap * 3) / 2;
    if (padded_size < cap) {
      return KHS_NULL;
    }
    cap = padded_size;
  }
  khs_table *table = khs_alloc(sizeof(khs_table) + sizeof(khs_val_pair) * cap);
  if (table == NULL) {
    return KHS_NULL;
  }
  table->cap = cap;
  table->ref_c = 1;
  for (khs_size i = 0; i < cap; i++) {
    table->entries[i] = (khs_val_pair){ KHS_NULL, KHS_NULL };
  }
  return (khs_val){ .type = KHS_TYPE_TABLE, .val.table = table, .managed = true, .len = 0 };
}

khs_val khs_static_table(khs_size cap, unsigned char *bytes, unsigned int bytes_size) {
  assert(sizeof(khs_table) + sizeof(khs_val_pair) * cap <= bytes_size);
  (void) bytes_size;
  khs_table *table = (khs_table *) bytes;
  table->cap = cap;
  table->ref_c = 1;
  for (khs_size i = 0; i < cap; i++) {
    table->entries[i] = (khs_val_pair){ KHS_NULL, KHS_NULL };
  }
  return (khs_val){ .type = KHS_TYPE_TABLE, .managed = false, .val.table = table, .len = 0 };
}

bool khs_table_should_grow(khs_val table, khs_size extra) {
  assert(KHS_TYPEOF(table) == KHS_TYPE_TABLE);
  khs_size expected_capacity = (table.len + extra) * 4 / 3;
  return table.val.table->cap < expected_capacity || expected_capacity < table.len;
}

khs_val khs_new_array(khs_size len, const khs_val *items, khs_size fit_for, bool padded) {
  assert(fit_for >= len);
  khs_size cap = padded ? (fit_for * 3 / 2) + 1 : fit_for;
  if (padded && cap <= fit_for) {
    return KHS_NULL;
  }
  khs_managed_array *array = khs_alloc(sizeof(khs_managed_array) + sizeof(khs_val) * cap);
  if (array == NULL) {
    return KHS_NULL;
  }
  array->cap = cap;
  array->ref_c = 1;
  if (items != NULL) {
    for (khs_size i = 0; i < len; i++) {
      array->items[i] = khs_retain(items[i]);
    }
  } 
  else {
    for (khs_size i = 0; i < len; i++) {
      array->items[i] = KHS_NULL;
    }
  }
  return (khs_val){ .type = KHS_TYPE_ARRAY, .len = len, .managed = true, .val.managed_array = array };
}

khs_size khs_get_array_capacity(khs_val val) {
  assert(KHS_TYPEOF(val) == KHS_TYPE_ARRAY);
  if (val.managed) {
    return val.val.managed_array->cap;
  }
  else {
    return KHS_LENOF(val);
  }
}

const khs_val *khs_get_raw_array(khs_val array) {
  assert(KHS_TYPEOF(array) == KHS_TYPE_ARRAY);
  if (array.managed) {
    return &array.val.managed_array->items[0];
  }
  return array.val.static_array;
}

bool khs_array_push(khs_val *array, khs_val val) {
  assert(KHS_TYPEOF(*array) == KHS_TYPE_ARRAY);
  assert(khs_get_refcount(*array) == 1);
  assert(array->managed);
  khs_managed_array *ma = array->val.managed_array;
  if (ma->cap == array->len) {
    khs_size new_cap = ma->cap * 3 / 2 + 1;
    if (new_cap <= ma->cap) {
      return false;
    }
    khs_managed_array *new_array = khs_alloc(sizeof(khs_managed_array) + sizeof(khs_val) * new_cap);
    if (new_array == NULL) {
      return false;
    }
    new_array->ref_c = 1;
    for (khs_size i = 0; i < array->len; i++) {
      new_array->items[i] = ma->items[i];
    }
    new_array->cap = new_cap;
    array->val.managed_array = new_array;
    khs_free(ma);
    ma = new_array;
  }
  assert(ma->cap > array->len);
  ma->items[array->len++] = khs_retain(val);
  return true;
}

khs_val_pair *khs_iter_table(khs_val table_v, khs_val_pair *iter) {
  assert(KHS_TYPEOF(table_v) == KHS_TYPE_TABLE);
  khs_table *table = table_v.val.table;
  khs_val_pair *table_end = table->entries + table->cap;
  assert((iter >= table->entries && iter < table_end) || iter == NULL);
  if (table_v.len == 0) {
    return NULL;
  }
  if (iter == NULL) {
    iter = table->entries - 1;
  }
  for (khs_val_pair *i = iter + 1; i < table_end; i++) {
    if (KHS_TYPEOF(i->key) != KHS_TYPE_NULL) {
      return i;
    }
  }
  return NULL;
}

static khs_val khs_parse_eval_expr(khs_lex_state *lex, bool eval, bool ignore_newlines);
static khs_val khs_parse_eval_all_exprs(khs_lex_state *lex, bool eval, unsigned char until_tok, khs_lex_token *end_tok);
static khs_val khs_parse_eval_args(khs_lex_state *lex, bool eval, bool ignore_newlines, khs_size *n_args);

static khs_val khs_parse_do_block(khs_lex_state *lex, khs_lex_token intial_token) {
  khs_lex_token end_tok;
  khs_val res = khs_parse_eval_all_exprs(lex, false, KHS_TOK_END, &end_tok);
  if(KHS_TYPEOF(res) == KHS_TYPE_ERR)
    return res;

  unsigned int len = end_tok.src - intial_token.src;
  if(len > KHS_SIZE_MAX) {
    khs_blame_range(lex, intial_token.src, end_tok.src + end_tok.len);
    return KHS_ERR("Function body too large");
  }
    
  khs_val fn = { .type = KHS_TYPE_FN, .val.fn = intial_token.src, .len = len };
  return fn;
}

static khs_val khs_parse_eval_fn_assign(khs_lex_state *lex, bool eval, khs_lex_token var_tok,  khs_lex_token fn_assign) {
  khs_val *args_begin = khs_save_arg_state();
  khs_stack_entry *assign_to_var;
  khs_val fn = KHS_NULL;
  if (eval) {
    assign_to_var = get_global(var_tok.content.sym.str, var_tok.content.sym.len);
    if (assign_to_var == NULL) {
      khs_blame_token(lex, var_tok);
      return KHS_ERR("Undeclared variable");
    }
    khs_stack_entry *fn_var = get_global(fn_assign.content.sym.str, fn_assign.content.sym.len);
    if (fn_var == NULL) {
      khs_blame_token(lex, fn_assign);
      return KHS_ERR("Unkown function");
    }
    fn = khs_retain(fn_var->val);
    if (!khs_push_arg(assign_to_var->val)) {
      khs_blame_token(lex, var_tok);
      return KHS_ERR("Argument stack overflow");
    }
    assign_to_var->val = KHS_NULL;
  }
  khs_val err;
  khs_size n_args;
  err = khs_parse_eval_args(lex, eval, false, &n_args);
  if (KHS_TYPEOF(err) == KHS_TYPE_ERR) {
    khs_blame_token(lex, var_tok);
    goto ERR;
  }
  if (!eval) {
    return KHS_NULL;
  }
  khs_val res = khs_call(fn, args_begin, n_args + 1, false);
  khs_restore_arg_state(args_begin, false);
  if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    khs_blame_token(lex, fn_assign);
  }
  else {
    assign_to_var->val = khs_retain(res);
  }
  return res;
ERR:
  khs_release(fn);
  khs_restore_arg_state(args_begin, true);
  return err;
}

static khs_val khs_parse_eval_term(khs_lex_state *lex, bool eval) {
  khs_lex_token tok = khs_lex_pop(lex);
  switch (tok.type) {
    case KHS_TOK_NUMBER:
      return KHS_NUMBER(tok.content.number);
    case KHS_TOK_STRING:
      return KHS_STATIC_STR(tok.content.sym.str, tok.content.sym.len);
    case KHS_TOK_OPEN_BRACKET: {
      if (khs_lex_accept(lex, KHS_TOK_CLOSE_BRACKET, NULL)) {
        return KHS_NULL;
      }
      khs_val res = khs_parse_eval_expr(lex, eval, true);
      if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
        return res;
      }
      khs_lex_token end_bracket = khs_lex_pop(lex);
      if (end_bracket.type != KHS_TOK_CLOSE_BRACKET) {
        khs_blame_token(lex, end_bracket);
        return KHS_ERR("Expected ')'");
      }
      return res; }
    case KHS_TOK_SYM:
    case KHS_TOK_OP: {
      khs_lex_token fn_assign;
      if (khs_lex_accept(lex, KHS_TOK_ASSIGN, NULL)) {
        khs_val assign_val = khs_parse_eval_expr(lex, eval, false);
        if (KHS_TYPEOF(assign_val) == KHS_TYPE_ERR) {
          return assign_val;
        }
        if (!eval) {
          return KHS_NULL;
        }
        khs_stack_entry *var = get_global(tok.content.sym.str, tok.content.sym.len);
        if (var == NULL) {
          khs_blame_token(lex, tok);
          khs_release(assign_val);
          return KHS_ERR("Undeclared variable");
        }
        if (var->is_const) {
          khs_blame_token(lex, tok);
          khs_release(assign_val);
          return KHS_ERR("Attempting to reassign constant variable");
        }
        khs_release(var->val);
        var->val = khs_retain(assign_val);
        return assign_val;
      } 
      else if(khs_lex_accept(lex, KHS_TOK_FN_ASSIGN, &fn_assign)) {
        return khs_parse_eval_fn_assign(lex, eval, tok, fn_assign);
      }
      if (!eval) {
        return KHS_NULL;
      }
      khs_stack_entry *var = get_global(tok.content.sym.str, tok.content.sym.len);
      if (var == NULL) {
        khs_blame_token(lex, tok);
        return KHS_ERR("Undeclared variable");
      }
      return khs_retain(var->val); }
    case KHS_TOK_LET: {
      bool global = khs_lex_accept(lex, KHS_TOK_GLOBAL, NULL);
      khs_lex_token var_sym = khs_lex_pop(lex);
      if (var_sym.type != KHS_TOK_SYM && var_sym.type != KHS_TOK_OP) {
        khs_blame_token(lex, var_sym);
        return KHS_ERR("Expected variable name");
      }
      khs_lex_token assign_tok = khs_lex_pop(lex);
      if (assign_tok.type != KHS_TOK_ASSIGN) {
        khs_blame_token(lex, assign_tok);
        return KHS_ERR("Expected '='");
      }
      khs_val assign_val = khs_parse_eval_expr(lex, eval, false);
      if (KHS_TYPEOF(assign_val) == KHS_TYPE_ERR) {
        return assign_val;
      }
      if(!eval) {
        return KHS_NULL;
      }
      const char *var_name = var_sym.content.sym.str;
      khs_size var_name_len = var_sym.content.sym.len;
      if (khs_get_local(var_name, var_name_len) != NULL) {
        khs_release(assign_val);
        khs_blame_token(lex, var_sym);
        return KHS_ERR("Redeclaration of variable");
      }
      if (CURRENT_NAMESPACE.start != NULL) {
        khs_scope_namespace namespace = global? (khs_scope_namespace){ NULL, NULL } : CURRENT_NAMESPACE;
        khs_stack_entry new_var = { var_name, var_name_len, assign_val, false, namespace };
        bool ok = khs_push_stack(&new_var);
        if (!ok) {
          khs_release(assign_val);
          khs_blame_token(lex, var_sym);
          return KHS_ERR("Out of variable space");
        }
      } 
      else {
        khs_stack_entry *var = khs_index_globals(var_name, var_name_len);
        if (var->name != NULL) {
          khs_release(assign_val);
          khs_blame_token(lex, var_sym);
          return KHS_ERR("Redeclaration of global variable");
        }
        var->name = var_name;
        var->name_len = var_name_len;
        var->val = khs_retain(assign_val);
        var->is_const = false;
      }
      return assign_val; }
    case KHS_TOK_FN: {
      while (!khs_lex_accept(lex, KHS_TOK_DO, NULL)) {
        khs_lex_token arg = khs_lex_pop(lex);
        if (arg.type == KHS_TOK_VARARGS) {
          khs_lex_token varargs_ident = khs_lex_pop(lex);
          if (varargs_ident.type != KHS_TOK_SYM && varargs_ident.type != KHS_TOK_OP) {
            khs_blame_token(lex, varargs_ident);
            return KHS_ERR("Expected variadic argument name");
          }
          khs_lex_token do_tok = khs_lex_pop(lex);
          if (do_tok.type != KHS_TOK_DO) {
            khs_blame_token(lex, do_tok);
            return KHS_ERR("Expected 'do' following final argument (variadic argument must be final argument)");
          }
          break;
        } 
        else if(arg.type != KHS_TOK_SYM && arg.type != KHS_TOK_OP) {
          khs_blame_token(lex, arg);
          return KHS_ERR("Expected argument name");
        }
      }
      return khs_parse_do_block(lex, tok); }
    case KHS_TOK_DO:
      return khs_parse_do_block(lex, tok);
    case KHS_TOK_ENDLINE:
      khs_blame_token(lex, tok);
      return KHS_ERR("Unexpected end of line");
    case KHS_TOK_ERR: {
      khs_blame_token(lex, tok);
      unsigned int len;
      const char *msg = khs_lex_err_str(tok, &len);
      return (khs_val){ .type = KHS_TYPE_ERR, .managed = false, .len = len, .val.str = msg }; }
    default:
      khs_blame_token(lex, tok);
      return KHS_ERR("Unexpected token");
  }
}

static khs_val khs_parse_eval_subexpr(khs_lex_state *lex, bool eval) {
  khs_val term = khs_parse_eval_term(lex, eval);
  if (KHS_TYPEOF(term) == KHS_TYPE_ERR) {
    return term;
  }
  khs_lex_token op;
  while (khs_lex_accept(lex, KHS_TOK_OP, &op) ) {
    khs_val next_term = khs_parse_eval_term(lex, eval);
    if (KHS_TYPEOF(next_term) == KHS_TYPE_ERR) {
      khs_release(term);
      return next_term;
    }
    if (!eval) {
      continue;
    }
    khs_stack_entry *op_var = get_global(op.content.sym.str, op.content.sym.len);
    if (op_var == NULL) {
      khs_blame_token(lex, op);
      khs_release(term);
      return KHS_ERR("Unkown variable");
    }
    khs_val op_fn = khs_retain(op_var->val);
    khs_val args[2] = { term, next_term };
    khs_val res = khs_call(op_fn, args, 2, false);
    term = res;
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
      khs_blame_token(lex, op);
      return res;
    }
  }
  return term;
}

static bool khs_continue_expr(khs_lex_state *lex) {
  switch (khs_lex_peek(lex).type) {
    case KHS_TOK_CLOSE_BRACKET:
    case KHS_TOK_EOF:
    case KHS_TOK_END:
      return false;
    default:
      return true;
  }
}

static khs_val khs_parse_eval_args(khs_lex_state *lex, bool eval, bool ignore_newlines, khs_size *out_n_args) {
  khs_size n_args = 0;
  while (true) {
    if (ignore_newlines) {
      khs_lex_accept(lex, KHS_TOK_ENDLINE, NULL);
    }
    else if (khs_lex_peek(lex).type == KHS_TOK_ENDLINE) {
      break;
    }
    if (!khs_continue_expr(lex)) {
      break;
    }
    khs_val res = khs_parse_eval_subexpr(lex, eval);
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
      return res;
    }
    if (!eval) {
      continue;
    }
    bool ok = khs_push_arg(res);
    if (!ok) {
      khs_release(res);
      return KHS_ERR("Argument stack overflow");
    }
    n_args++;
  }
  *out_n_args = n_args;
  return KHS_NULL;
}

static khs_val khs_parse_eval_expr(khs_lex_state *lex, bool eval, bool ignore_newlines) {
  khs_val err;
  khs_lex_token fn_tok = khs_lex_peek(lex);
  static unsigned expr_recursion_counter = 0;
  expr_recursion_counter++;
  khs_val *args_begin = khs_save_arg_state();
  if (expr_recursion_counter > KHS_MAX_EXPR_RECURSION) {
    khs_blame_token(lex, fn_tok);
    err = KHS_ERR("Expression recursion limit reached");
    goto ERR;
  }
  khs_val fn = khs_parse_eval_subexpr(lex, eval); 
  if (KHS_TYPEOF(fn) == KHS_TYPE_ERR) {
    err = fn;
    goto ERR;
  }
  khs_size n_args;
  khs_val arg_res = khs_parse_eval_args(lex, eval, ignore_newlines, &n_args);
  if (KHS_TYPEOF(arg_res) == KHS_TYPE_ERR) {
    err = arg_res;
    goto ERR;
  }
  expr_recursion_counter--;
  if (!eval) {
    return KHS_NULL;
  }
  if (n_args == 0) {
    return fn;
  }
  khs_val res = khs_call(fn, args_begin, n_args, false);
  if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    khs_blame_token(lex, fn_tok);
  }
  khs_restore_arg_state(args_begin, false);
  return res; 
ERR:
  expr_recursion_counter--;
  khs_restore_arg_state(args_begin, true);
  return err;
}

khs_val khs_parse_eval_all_exprs(khs_lex_state *lex, bool eval, unsigned char until_tok, khs_lex_token *end_tok) {
  khs_val res = KHS_NULL;
  khs_lex_accept(lex, KHS_TOK_ENDLINE, NULL);
  while (!khs_lex_accept(lex, until_tok, end_tok)) {
    khs_release(res);
    res = khs_parse_eval_expr(lex, eval, false);
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
      break;
    }
    khs_lex_accept(lex, KHS_TOK_ENDLINE, NULL);
  }
  return res;
}

khs_size khs_get_fn_arity(khs_val fn, bool *variadic) {
  assert(KHS_TYPEOF(fn) == KHS_TYPE_FN);
  khs_lex_state lex;
  khs_lex_state_init(&lex, fn.val.fn, fn.len);
  khs_lex_accept(&lex, KHS_TOK_FN, NULL);
  *variadic = false;
  khs_size n_args = 0;
  while (!khs_lex_accept(&lex, KHS_TOK_DO, NULL)) {
    if (khs_lex_accept(&lex, KHS_TOK_VARARGS, NULL)) {
      *variadic = true;
      khs_lex_token varargs_name = khs_lex_pop(&lex);
      assert(varargs_name.type == KHS_TOK_SYM || varargs_name.type == KHS_TOK_OP);
      assert(khs_lex_accept(&lex, KHS_TOK_DO, NULL));
      break;
    }
    khs_lex_token arg_name = khs_lex_pop(&lex);
    assert(arg_name.type == KHS_TOK_SYM || arg_name.type == KHS_TOK_OP);
    n_args++;
  }
  return n_args;
}

khs_val khs_call_internal_fn(khs_val fn, const khs_val *args, khs_size n_args) {
  assert(KHS_TYPEOF(fn) == KHS_TYPE_FN);
  khs_val err;
  bool is_variadic;
  khs_size arity = khs_get_fn_arity(fn, &is_variadic);
  if (is_variadic) {
    if (n_args < arity) {
      return KHS_ERR("Not enough arguments provided");
    }
  } 
  else if(n_args != arity) {
    return KHS_ERR("Wrong number of arguments");
  }
  khs_scope_namespace prev_namespace = CURRENT_NAMESPACE;
  CURRENT_NAMESPACE = (khs_scope_namespace){ fn.val.fn, fn.val.fn + fn.len };
  khs_stack_entry *prev_scope = khs_enter_scope();
  khs_lex_state lex;
  khs_lex_state_init(&lex, fn.val.fn, fn.len);
  khs_lex_accept(&lex, KHS_TOK_FN, NULL);
  for (khs_size i = 0; i < arity; i++) {
    khs_lex_token arg_tok = khs_lex_pop(&lex);
    assert(arg_tok.type == KHS_TOK_SYM || arg_tok.type == KHS_TOK_OP);
    const char *arg_name = arg_tok.content.sym.str;
    khs_size arg_name_len = arg_tok.content.sym.len;
    if (khs_get_local(arg_name, arg_name_len) != NULL) {
      khs_blame_token(&lex, arg_tok);
      err = KHS_ERR("Redeclaration of variable");
      goto ERR;
    }
    khs_stack_entry arg_var = { arg_name, arg_name_len, args[i], false, CURRENT_NAMESPACE };
    bool ok = khs_push_stack(&arg_var);
    khs_release(args[i]);
    if (!ok) {
      khs_blame_token(&lex, arg_tok);
      err = KHS_ERR("Out of variable space");
      goto ERR;
    }
  }
  if (is_variadic) {
    khs_lex_token variadic_tok = khs_lex_pop(&lex);
    khs_lex_token varargs_name = khs_lex_pop(&lex);
    assert(variadic_tok.type == KHS_TOK_VARARGS);
    assert(varargs_name.type == KHS_TOK_SYM || varargs_name.type == KHS_TOK_OP);
    if (khs_get_local(varargs_name.content.sym.str, varargs_name.content.sym.len) != NULL) {
      khs_blame_token(&lex, varargs_name);
      err = KHS_ERR("Redeclaration of variable");
      goto ERR;
    }
    assert(n_args >= arity);
    khs_size n_varargs = n_args - arity;
    assert(args + arity + n_varargs == args + n_args);
    khs_val varargs_array = khs_new_array(n_varargs, args + arity, n_varargs, false);
    khs_release_values(args + arity, n_varargs);
    if (KHS_TYPEOF(varargs_array) == KHS_TYPE_NULL) {
      err = KHS_ERR("Out of memory; cannot construct variadic arguments array");
      goto ERR;
    }
    khs_stack_entry arg_var = { varargs_name.content.sym.str, varargs_name.content.sym.len, varargs_array, false, CURRENT_NAMESPACE }; //Push the var to the stack
    bool ok = khs_push_stack(&arg_var);
    khs_release(varargs_array);
    if (!ok) {
      khs_blame_token(&lex, varargs_name);
      err = KHS_ERR("Out of variable space");
      goto ERR;
    }
  }
  bool ok = khs_lex_accept(&lex, KHS_TOK_DO, NULL);
  assert(ok);
  khs_val res = khs_parse_eval_all_exprs(&lex, true, KHS_TOK_EOF, NULL);
  CURRENT_NAMESPACE = prev_namespace;
  khs_leave_scope(prev_scope);
  return res;
ERR:
  CURRENT_NAMESPACE = prev_namespace;
  khs_leave_scope(prev_scope);
  return err;
}

khs_val khs_call(khs_val fn, const khs_val *args, unsigned int n_args, bool borrow) {
  khs_val err;
  if (borrow) {
    khs_retain(fn);
    khs_retain_values(args, n_args);
  }  
  if (n_args > KHS_SIZE_MAX) {
    err = KHS_ERR("Too many arguments");
    goto ERR;
  }
  switch (fn.type) {
    case KHS_TYPE_EXT_FN: {
      khs_external_fn *ext_fn = fn.val.ext_fn;
      if (ext_fn->arity < 0) {
        if (n_args < (unsigned int)-(ext_fn->arity + 1)) {
          err = KHS_ERR("Not enough arguments");
          goto ERR;
        }
      } 
      else if (n_args != (unsigned int)ext_fn->arity) {
        err = KHS_ERR("Wrong number of arguments");
        goto ERR;
      }
      khs_val res = ext_fn->fn(args, n_args);
      if (ext_fn->auto_release) {
        khs_release_values(args, n_args);
      }
      khs_release(fn);
      
      if(KHS_TYPEOF(res) == KHS_TYPE_ERR)
        khs_blame_name(ext_fn->name, ext_fn->name_len);
      return res; } 
      break;
    case KHS_TYPE_FN: {
      khs_val res = khs_call_internal_fn(fn, args, n_args);
      khs_release(fn);
      return res;
    }
    case KHS_TYPE_TABLE: {
      if (n_args == 0) {
        err = KHS_ERR("Cannot index table without key");
        goto ERR;
      } 
      if (n_args == 1) {
        khs_val res = index_table(fn, args[0]);
        khs_release(args[0]);
        khs_release(fn);
        return res;
      }
      khs_val member = index_table(fn, args[0]);
      khs_release(args[0]);
      khs_stack_entry *prev_scope = khs_enter_scope();
      khs_stack_entry self_var = { "self", sizeof("self") - 1, fn, false, { NULL, NULL } };
      bool ok = khs_push_stack(&self_var);
      khs_release(fn);
      if (!ok) {
        khs_leave_scope(prev_scope);
        khs_release(member);
        khs_release_values(args + 1, n_args - 1);
        return KHS_ERR("Out of variable space");
      }
      khs_val res = khs_call(member, args + 1, n_args - 1, false);
      khs_leave_scope(prev_scope);
      return res; }
    case KHS_TYPE_ARRAY: {
      if (n_args != 1) {
        err = KHS_ERR("Can only index array with an index");
        goto ERR;
      }
      if (KHS_TYPEOF(args[0]) != KHS_TYPE_NUMBER) {
        khs_blame_arg(args[0]);
        err = KHS_ERR("Can only index array with a number");
        goto ERR;
      }
      khs_float f = khs_as_num(args[0]);
      if (!khs_is_integer(args[0])) {
        khs_blame_arg(args[0]);
        err = KHS_ERR("Can only index array with an integer number");
        goto ERR;
      }
      khs_size index = f;
      if (f > KHS_SIZE_MAX || index >= KHS_LENOF(fn)) {
        khs_release(fn);
        khs_release(args[0]);
        return KHS_NULL;
      }
      khs_val res = khs_retain(khs_get_raw_array(fn)[index]);
      khs_release(fn);
      khs_release(args[0]);
      return res; }
    case KHS_TYPE_NULL: {
      khs_release_values(args, n_args);
      return KHS_NULL; }
    case KHS_TYPE_OBJECT: {
      khs_object *obj = khs_as_object(fn);
      if (obj->obj_class->call == NULL) {
        khs_blame_arg(fn);
        err = KHS_ERR("Attempting to call non-callable object");
        goto ERR;
      }
      khs_val res = obj->obj_class->call(obj, args, n_args);
      khs_release(fn);
      khs_release_values(args, n_args);
      return res;
    }
    default:
      khs_blame_arg(fn);
      err = KHS_ERR("Attempting to call non-function value");
      goto ERR;
  }
ERR:
  khs_release(fn);
  khs_release_values(args, n_args);
  return err;
}

khs_val khs_pcall(khs_val fn, const khs_val *args, unsigned int n_args, bool borrow, bool print_trace) {
  khs_val res = khs_call(fn, args, n_args, borrow);
  if (print_trace && KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    khs_log_err(res);
  }
  khs_clear_err();
  return res;
}


khs_val khs_eval(const char *src, unsigned int src_len, khs_err_action err) {
  khs_lex_state lex;
  khs_lex_state_init(&lex, src, src_len);
  khs_stack_entry *prev_scope = khs_enter_scope();
  khs_scope_namespace prev_namespace = CURRENT_NAMESPACE;
  CURRENT_NAMESPACE = (khs_scope_namespace){ NULL, NULL };
  khs_val res = khs_parse_eval_all_exprs(&lex, true, KHS_TOK_EOF, NULL);
  khs_leave_scope(prev_scope);
  CURRENT_NAMESPACE = prev_namespace;
  if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    if (err == KHS_PRINT_ERR) {
      khs_log_err(res);
      khs_clear_err();
    } 
    else if(err == KHS_CATCH_ERR) {
      khs_clear_err();
    }
  }
  return res;
}

void khs_clear() {
  assert(STACK_BASE == stack_start);
  for (khs_stack_entry *p = stack_top - 1; p >= stack_start; p--) {
    khs_release(p->val);
  }
  for (unsigned int i = 0; i < KHS_GLOBALS_LOOKUP_TABLE_SIZE; i++) {
    khs_stack_entry *entry = &GLOBALS_LOOKUP[i];
    entry->name = NULL;
    khs_release(entry->val);
    entry->is_const = false;
  }
  stack_top = STACK_BASE;
}

bool khs_load_included_libs() {
  khs_load_debug_lib();
  khs_load_unix_lib();
  khs_load_io_lib();
  return khs_load_core_lib();
}

