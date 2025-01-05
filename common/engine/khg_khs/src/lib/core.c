#include "khg_khs/khs.h"
#include "khg_khs/lib/lib.h"
#include "khg_khs/util.h"
#include <assert.h>

#define KHS_EXPECT_TYPE_I(index, type, name, ordinal_name) {\
  assert(n_args > index);\
  if(KHS_TYPEOF(args[index]) != type) {\
    khs_blame_arg(args[index]);\
    return KHS_ERR("Expected " name " as " ordinal_name " argument, got '%0'");\
  }\
}

#define KHS_EXPECT_TYPE1(t1, name1)\
{\
  KHS_EXPECT_TYPE_I(0, t1, name1, "first");\
}

#define KHS_EXPECT_TYPE2(t1, name1, t2, name2)\
{\
  KHS_EXPECT_TYPE1(t1, name1);\
  KHS_EXPECT_TYPE_I(1, t2, name2, "second");\
}

#define KHS_EXPECT_TYPE3(t1, name1, t2, name2, t3, name3) {\
  KHS_EXPECT_TYPE2(t1, name1, t2, name2);\
  KHS_EXPECT_TYPE_I(2, t3, name3, "third");\
}

#define KHS_DEF_FN(name, args, ...) khs_set_var(name, sizeof(name) - 1, KHS_LITERAL_FN(args, __VA_ARGS__), true)

#define KHS_TAPE_LEN (sizeof(TAPE) / sizeof(TAPE[0]))
#define KHS_LSTR(str) str, sizeof(str) - 1

#define KHS_STR_BUFF_START_SIZE 16

typedef struct khs_str_buff {
  char *str, *top, *end;
} khs_str_buff;

typedef struct khs_stringf {
  char *str, *end, *at;
} khs_stringf;

static khs_val ELSE_TAG, ELSEIF_TAG;

typedef struct khs_eval_item khs_eval_item;
static struct khs_eval_item {
  khs_eval_item *prev;
  khs_val item;
} *EVAL_LIST = NULL;

static khs_val CATCH_TAG;
static khs_val PRINT_TRACE_TAG;

static khs_val ARRAYOF_ARRAY = KHS_NULL;

static bool SORT_ERR = false;

static unsigned long long TAPE[] = { 59243892, 2014914089654949231, 120301499583420, 23, 3230239239, 120302102103, 904924490212, 10412  };

static void khs_mcpy(void *to_ptr, const void *from_ptr, size_t n) {
  char *to = to_ptr;
  const char *from = from_ptr;
  while(n--) {
    *(to++) = *(from++);
  }
}

KHS_MATH_OP(khs_add_callback, 0, 0, +) 
KHS_MATH_OP(khs_mul_callback, 1, 0, *)
KHS_MATH_OP(khs_div_callback, khs_as_num(args[0]), 1, /)

static khs_val khs_sub_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_NUMBER) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected number as first argument for '-'");
  }
  khs_float res = khs_as_num(args[0]);
  if (n_args == 1) {
    return KHS_NUMBER(-res);
  }
  for (khs_size i = 1; i < n_args; i++) {
    if(KHS_TYPEOF(args[i]) != KHS_TYPE_NUMBER) {
      khs_blame_arg(args[i]);
      return KHS_ERR("Expected number as argument for '-'");
    }
    res -= khs_as_num(args[i]);
  }
  return KHS_NUMBER(res);
}

static khs_val khs_if_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_BOOL) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected boolean as if condition");
  }
  if (khs_as_bool(args[0])) {
    return khs_call(args[1], NULL, 0, true);
  }
  assert(n_args >= 2);
  n_args -= 2;
  args += 2;
  while (n_args > 0) {
    khs_val tag;
    KHS_POP_ARG(tag, "");
    if (khs_val_cmp(tag, ELSEIF_TAG) == 0) {
      khs_val cond;
      KHS_POP_ARG(cond, "Expected condition following 'elseif'");
      KHS_EXPECT_TYPE(cond, KHS_TYPE_BOOL, "Expected boolean condition following 'elseif'");
      khs_val then_do;
      KHS_POP_ARG(then_do, "Expected argument following 'elseif'");
      if (khs_as_bool(cond)) {
        return khs_call(then_do, NULL, 0, true);
      }
    } 
    else if (khs_val_cmp(tag, ELSE_TAG) == 0) {
      khs_val then_do;
      KHS_POP_ARG(then_do, "Expected argument following 'else'");
      return khs_call(then_do, NULL, 0, true);
    } 
    else {
      khs_blame_arg(tag);
      return KHS_ERR("Unexpected argument for 'if'. Expected either 'elseif' or 'else'");
    }
  }
  if (n_args != 0) {
    khs_val arg;
    KHS_POP_ARG(arg, "");
    khs_blame_arg(arg);
    return KHS_ERR("Expected no more arguments after 'else'");
  }
  return KHS_NULL;
}

static khs_val khs_eq_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  return KHS_BOOL(khs_val_cmp(args[0], args[1]) == 0);
}

static khs_val khs_not_eq_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  return KHS_BOOL(khs_val_cmp(args[0], args[1]) != 0);
}

static khs_val khs_not_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_BOOL) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected boolean as argument to 'not'");
  }
  return KHS_BOOL(!khs_as_bool(args[0]));
}

static khs_val khs_array_callback(const khs_val *args, khs_size n_args) {
  khs_val array = khs_new_array(n_args, args, n_args, false);
  if (KHS_TYPEOF(array) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  return array;
}

static khs_val khs_for_in_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  khs_val index = KHS_NUMBER(0);
  khs_val res = KHS_NULL;
  while (true) {
    khs_val index_res = khs_call(args[0], &index, 1, true);
    if (KHS_TYPEOF(index_res) == KHS_TYPE_ERR) {
      return index_res;
    }
    if (KHS_TYPEOF(index_res) == KHS_TYPE_NULL) {
      break;
    }
    khs_release(res);
    res = khs_call(khs_retain(args[1]), &index_res, 1, false);
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
      return res;
    }
    index = KHS_NUMBER(khs_as_num(index) + 1);
  }
  return res;
}

static khs_val khs_for_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_NUMBER) {
    khs_blame_arg(args[0]);
    return KHS_ERR("First argument of 'for' must be number");
  }
  if (KHS_TYPEOF(args[1]) != KHS_TYPE_NUMBER) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Second argument of 'for' must be number");
  }
  khs_float from = khs_as_num(args[0]);
  khs_float until = khs_as_num(args[1]);
  khs_float i = from;
  khs_val res = KHS_NULL;
  if (from <= until) {
    while (i < until) {
      khs_val index = KHS_NUMBER(i++);
      khs_release(res);
      res = khs_call(args[2], &index, 1, true);
      khs_release(index);
      if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
        return res;
      }
    }
  } 
  else {
    while (i > until) {
      khs_val index = KHS_NUMBER(i--);
      khs_release(res);
      res = khs_call(args[2], &index, 1, true);
      khs_release(index);
      if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
        return res;
      }
    }
  }
  return res;
}

static khs_val khs_table_callback(const khs_val *args, khs_size n_args) {
  if (n_args % 2 != 0) {
    return KHS_ERR("Table function only accepts an even number of arguments");
  }
  khs_val table = khs_new_table(n_args / 2, true);
  if (KHS_TYPEOF(table) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  for (khs_size i = 0; i < n_args; i += 2) {
    assert(i + 1 < n_args);
    int res = khs_table_insert(&table, args[i], args[i + 1], false);
    if (res != 0) {
      khs_blame_arg(args[i]);
      khs_release(table);
      switch (res) {
        case 3: 
          return KHS_ERR("Value is not valid key");
        case 2:
          return KHS_ERR("Duplicate key");
        default:
          assert(false);
      }
    }
  }
  return table;
}

static khs_val khs_tag_callback(const khs_val *args, khs_size n_args) {
  (void)n_args, (void)args;
  khs_val tag = khs_new_tag();
  if (KHS_TYPEOF(tag) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of tags");
  }
  return tag;
}

static khs_val khs_invoke_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  khs_val res = khs_call(args[0], NULL, 0, false);
  return res;
}

static khs_val khs_assert_callback(const khs_val *args, khs_size n_args) {
  if (n_args != 1 && n_args != 2) {
    return KHS_ERR("Assert takes either one or two arguments");
  }
  if (n_args == 2 && KHS_TYPEOF(args[1]) != KHS_TYPE_STR) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Expected string message as second argument for 'assert'");
  }
  bool passed_assertion;
  if (KHS_TYPEOF(args[0]) == KHS_TYPE_BOOL) {
    passed_assertion = khs_as_bool(args[0]);
  }
  else {
    passed_assertion = KHS_TYPEOF(args[0]) != KHS_TYPE_NULL;
  }
  if (!passed_assertion) {
    if (n_args == 1) {
      return KHS_ERR("Assertion failed");
    }
    return khs_str_as_err(args[1]);
  }
  return khs_retain(args[0]);
}

KHS_BOOL_OP(khs_and_callback, "and?", &&)
KHS_BOOL_OP(khs_or_callback, "or?", ||)

KHS_CMP_OP(khs_less_callback, 
  int cmp_res = khs_val_cmp(prev, next);
  if (cmp_res == 0 || cmp_res == -1) {
    return KHS_FALSE;
  }
  if (cmp_res == 2) {
    khs_blame_arg(prev);
    khs_blame_arg(next);
    return KHS_ERR("Uncomparable values");
  }
)

KHS_CMP_OP(khs_greater_callback,
  int cmp_res = khs_val_cmp(prev, next);
  if (cmp_res == 0 || cmp_res == 1) {
    return KHS_FALSE;
  }
  if (cmp_res == 2) {
    khs_blame_arg(prev);
    khs_blame_arg(next);
    return KHS_ERR("Uncomparable values");
  }
)

KHS_CMP_OP(khs_less_eq_callback,
  int cmp_res = khs_val_cmp(prev, next);
  if (cmp_res == -1) {
    return KHS_FALSE;
  }
  if (cmp_res == 2) {
    khs_blame_arg(prev);
    khs_blame_arg(next);
    return KHS_ERR("Uncomparable values");
  }
)

KHS_CMP_OP(khs_greater_eq_callback,
  int cmp_res = khs_val_cmp(prev, next);
  if(cmp_res == 1) {
    return KHS_FALSE;
  }
  if(cmp_res == 2) {
    khs_blame_arg(prev);
    khs_blame_arg(next);
    return KHS_ERR("Uncomparable values");
  }
)

static khs_val khs_foreach_in_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  switch (KHS_TYPEOF(args[0])) {
    case KHS_TYPE_TABLE: {
      khs_val_pair *iter = NULL;
      khs_val res = KHS_NULL;
      while ((iter = khs_iter_table(args[0], iter))) {
        khs_release(res);
        khs_val iter_args[] = { iter->key, iter->val };
        res = khs_call(args[1], iter_args, 2, true);
        if (KHS_TYPEOF(res) == KHS_TYPE_ERR)
          return res;
      }
      return res; }
    case KHS_TYPE_ARRAY: {
      khs_val res = KHS_NULL;
      khs_size len = KHS_LENOF(args[0]);
      const khs_val *items = khs_get_raw_array(args[0]);
      for (khs_size i = 0; i < len; i++) {
        khs_release(res);
        khs_val iter_args[] = { KHS_NUMBER(i), items[i] };
        res = khs_call(args[1], iter_args, 2, true);
        if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
          return res;
        }
      }
      return res; }
    default:
      khs_blame_arg(args[0]);
      return KHS_ERR("Expected array or table as argument for 'foreach-in'");
  }
}

static void khs_union_tables(khs_val *into_table, khs_val from_table) {
  assert(khs_get_refcount(*into_table) == 1);
  khs_size remaining_cap = into_table->val.table->cap - KHS_LENOF(*into_table);
  assert(KHS_LENOF(from_table) <= remaining_cap); (void) remaining_cap;
  khs_val_pair *iter = NULL;
  while ((iter = khs_iter_table(from_table, iter))) {
    khs_table_insert(into_table, iter->key, iter->val, false);
  }
}

static khs_val khs_insert_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_TABLE) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected table as first argument for 'insert'");
  }
  khs_val table = args[0];
  khs_refc refs = khs_get_refcount(table);
  bool retain_table = false;
  if (refs != 1) {
    khs_val new_table = khs_new_table(KHS_LENOF(table) + 1, true);
    if (KHS_TYPEOF(new_table) == KHS_TYPE_NULL) {
      return KHS_ERR("Out of memory");
    }
    khs_union_tables(&new_table, args[0]);
    table = new_table;
  } 
  else {
    if (khs_table_should_grow(table, 1)) {
      khs_val new_table = khs_new_table(KHS_LENOF(table) + 1, true);
      if (KHS_TYPEOF(new_table) == KHS_TYPE_NULL)
        return KHS_ERR("Out of memory");
      khs_union_tables(&new_table, table);
      table = new_table;
    } 
    else {
      retain_table = true;
    }
  }
  int err = khs_table_insert(&table, args[1], args[2], false);
  if (retain_table) {
    khs_retain(table);
  }
  if (err == 0) {
    return table;
  }
  khs_release(table);
  switch (err) {
    case 3:
      khs_blame_arg(args[1]);
      return KHS_ERR("Invalid table key");
    case 2:
      khs_blame_arg(args[1]);
      return KHS_ERR("Duplicate key");
    case 1:
      assert(false);
      return KHS_NULL;
    default:
      assert(false);
      return KHS_NULL;
  }
}

static khs_val khs_union_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_TABLE) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected table as first argument for 'union:'");
  }
  if (KHS_TYPEOF(args[1]) != KHS_TYPE_TABLE) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Expected table as second argument for 'union:'");
  }
  khs_size a_len = KHS_LENOF(args[0]);
  khs_size b_len = KHS_LENOF(args[1]);
  if (KHS_SIZE_MAX - a_len < b_len) {
    return KHS_ERR("Out of memory");
  }
  khs_size total_len = a_len + b_len;
  khs_val new_table = khs_new_table(total_len, true);
  if (KHS_TYPEOF(new_table) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  khs_union_tables(&new_table, args[0]);
  khs_union_tables(&new_table, args[1]);
  return new_table;
}

static khs_val khs_concat_callback(const khs_val *args, khs_size n_args) {
  khs_val err;
  bool only_strings = true;
  for (khs_size i = 0; i < n_args; i++) {
    if (KHS_TYPEOF(args[i]) != KHS_TYPE_STR) {
      only_strings = false;
    }
  }
  const khs_val *strings = args;
  khs_val *str_array = NULL;
  if (!only_strings) {
    str_array = khs_talloc(n_args * sizeof(khs_val));
    for (khs_size i = 0; i < n_args; i++) {
      str_array[i] = khs_val_as_string(args[i]);
    }
    strings = str_array;
  }
  khs_size total_len = 0;
  for (khs_size i = 0; i < n_args; i++) {
    if (KHS_TYPEOF(strings[i]) == KHS_TYPE_ERR) {
      err = khs_retain(strings[i]);
      goto ERR;
    }
    total_len += KHS_LENOF(strings[i]);
  }
  khs_val res_str = khs_new_string(total_len, NULL);
  if (KHS_TYPEOF(res_str) == KHS_TYPE_NULL) {
    err = KHS_ERR("Out of memory");
    goto ERR;
  }
  char *raw_str = (char *) khs_get_raw_str(&res_str);
  char *s = raw_str;
  for (khs_size i = 0; i < n_args; i++) {
    khs_size len = KHS_LENOF(strings[i]);
    if (len > 0) {
      assert (s < raw_str + total_len);
    }
    khs_mcpy(s, khs_get_raw_str(&strings[i]), len);
    s += len;
  }
  if (str_array != NULL) {
    for(size_t i = 0; i < n_args; i++)
      khs_release(str_array[i]);
    khs_tfree(str_array);
  }
  return res_str;
ERR:
  if (str_array != NULL) {
    for (size_t i = 0; i < n_args; i++) {
      khs_release(str_array[i]);
    }
    khs_tfree(str_array);
  }
  return err;
}

static khs_val khs_in_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  khs_val index_res = khs_call(args[1], &args[0], 1, false);
  if (KHS_TYPEOF(index_res) == KHS_TYPE_ERR) {
    return index_res;
  }
  khs_val res = KHS_BOOL(KHS_TYPEOF(index_res) != KHS_TYPE_NULL);
  khs_release(index_res);
  return res;
}

static khs_val khs_sizeof_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  switch (KHS_TYPEOF(args[0])) {
    case KHS_TYPE_STR:
    case KHS_TYPE_TABLE:
    case KHS_TYPE_ARRAY:
      return KHS_NUMBER(KHS_LENOF(args[0]));
    default:
      return KHS_NUMBER(1);
  }
}

static khs_val khs_replace_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  switch (KHS_TYPEOF(args[0])) {
    case KHS_TYPE_TABLE: {
      khs_val index_res = khs_pcall(args[0], &args[1], 1, true, false); 
      if (KHS_TYPEOF(index_res) == KHS_TYPE_NULL) {
        khs_blame_arg(args[1]);
        khs_release(index_res);
        return KHS_ERR("Key not in struct");
      }
      khs_release(index_res);
      if (khs_get_refcount(args[0]) == 1) {
        khs_val res = args[0];
        khs_table_insert(&res, args[1], args[2], true);
        return khs_retain(res);
      } 
      else {
        khs_val new_table = khs_new_table(KHS_LENOF(args[0]) + 1, true);
        if (KHS_TYPEOF(new_table) == KHS_TYPE_NULL)  {
          return KHS_ERR("Out of memory");
        }
        khs_table_insert(&new_table, args[1], args[2], false);
        khs_union_tables(&new_table, args[0]);
        return new_table;
      } }
    default:
      khs_blame_arg(args[0]);
      return KHS_ERR("Can only use 'replace' on structs");
  }
}

void khs_core_lib_clear_evals() {
  khs_eval_item *next = NULL;
  for (khs_eval_item *p = EVAL_LIST; p != NULL; p = next) {
    next = p->prev;
    khs_release(p->item);
    khs_free(p);
  }
}

static khs_val khs_try_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  int mode;
  if (khs_val_cmp(args[1], CATCH_TAG) == 0) {
    mode = 0;
  } 
  else if (khs_val_cmp(args[1], ELSE_TAG) == 0) {
    mode = 1;
  } 
  else if (khs_val_cmp(args[1], PRINT_TRACE_TAG) == 0) {
    mode = 2;
  } 
  else {
    khs_blame_arg(args[1]);
    return KHS_ERR("Expected 'catch' or 'else'");
  }
  khs_val res = khs_pcall(args[0], NULL, 0, true, mode == 2);
  if (KHS_TYPEOF(res) != KHS_TYPE_ERR) {
    return res;
  }
  if (mode == 1) {
    khs_release(res);
    return khs_retain(args[2]);
  } 
  else {
    khs_val err_msg = khs_err_as_str(res);
    khs_release(res);
    khs_val res = khs_call(khs_retain(args[2]), &err_msg, 1, false);
    return res;
  }
}

static khs_val khs_eval_callback(const khs_val *args, khs_size n_args) {
  if (n_args != 1 && n_args != 3) {
    return KHS_ERR("`eval` takes either 1 or 3 arguments");
  }
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected string as argument for 'eval'");
  }
  khs_err_action err_action = KHS_PROP_ERR;
  if (n_args == 3) {
    if (khs_val_cmp(args[1], CATCH_TAG) == 0) {
      err_action = KHS_CATCH_ERR;
    } 
    else if (khs_val_cmp(args[1], PRINT_TRACE_TAG) == 0) {
      err_action = KHS_PRINT_ERR;
    } 
    else {
      khs_blame_arg(args[1]);
      return KHS_ERR("Expected 'catch'");
    }
  }
  khs_eval_item *new_entry = khs_alloc(sizeof(khs_eval_item));
  if (new_entry == NULL) {
    return KHS_ERR("Out of memory");
  }
  new_entry->prev = EVAL_LIST;
  new_entry->item = khs_retain(args[0]);
  EVAL_LIST = new_entry;
  khs_val res = khs_eval(khs_get_raw_str(&EVAL_LIST->item), KHS_LENOF(EVAL_LIST->item), err_action);
  if (err_action != KHS_PROP_ERR && KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    khs_val handler_res = khs_call(khs_retain(args[2]), &res, 1, false);
    return handler_res;
  }
  return res;
}

static khs_val khs_filter_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[0]);
    khs_release_values(args, n_args);
    return KHS_ERR("First argument of 'filter' must be array");
  }
  khs_val res;
  const khs_val *from_array = khs_get_raw_array(args[0]);
  if (khs_get_refcount(args[0]) == 1) {
    res = khs_retain(args[0]);
  }
  else {
    khs_size len = KHS_LENOF(args[0]);
    res = khs_new_array(len, NULL, len, false);
    if (KHS_TYPEOF(res) == KHS_TYPE_NULL) {
      khs_release_values(args, n_args);
      return KHS_ERR("Out of memory");
    }
    khs_val *a = (khs_val *)khs_get_raw_array(res);
    for (khs_size i = 0; i < len; i++) {
      a[i] = KHS_NULL;
    }
  }
  khs_val *to_array = (khs_val *)khs_get_raw_array(res);
  khs_size res_n = 0;
  for (khs_size i = 0; i < KHS_LENOF(args[0]); i++) {
    khs_val filter_res = khs_call(args[1], &from_array[i], 1, true);
    if (KHS_TYPEOF(filter_res) == KHS_TYPE_ERR) {
      khs_release(res);
      khs_release_values(args, n_args);
      return filter_res;
    } 
    else if (KHS_TYPEOF(filter_res) != KHS_TYPE_BOOL) {
      khs_release(res);
      khs_blame_arg(filter_res);
      khs_release(filter_res);
      khs_release_values(args, n_args);
      return KHS_ERR("Expected filter function to return boolean");
    }
    if (khs_as_bool(filter_res)) {
      khs_val tmp = khs_retain(from_array[i]);
      khs_release(*to_array);
      *(to_array++) = tmp; 
      res_n++;
    }
  }
  khs_release_values(args, n_args);
  res.len = res_n;
  return res;
}

static khs_val khs_push_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[0]);
    return KHS_ERR("First argument of push must be array");
  }
  khs_size len = KHS_LENOF(args[0]);
  khs_val array = KHS_NULL;
  if (khs_get_refcount(args[0]) == 1) {
    assert(args[0].managed);
    if (khs_get_array_capacity(args[0]) > len) {
      array =  khs_retain(args[0]);
    }
    else {
      array = khs_new_array(len, khs_get_raw_array(args[0]), len + 1, true);
    }
  } 
  else {
    array = khs_new_array(len, khs_get_raw_array(args[0]), len + 1, true);
  }
  if (KHS_TYPEOF(array) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  assert(array.managed && KHS_TYPEOF(array) == KHS_TYPE_ARRAY);
  assert(khs_get_array_capacity(array) > KHS_LENOF(array));
  khs_val *items = (khs_val*) khs_get_raw_array(array);
  items[array.len++] = khs_retain(args[1]);
  return array;
}

static khs_val khs_mod_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (!khs_is_integer(args[0])) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Can only take modulus of integer numbers");
  }
  if (!khs_is_integer(args[1])) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Can only take modulus of integer numbers");
  }
  long long t = khs_as_num(args[0]);
  long long n = khs_as_num(args[1]);
  long long res = ((t % n) + n) % n;
  return KHS_NUMBER(res);
}

static khs_val khs_arrayof_capture_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(ARRAYOF_ARRAY) == KHS_TYPE_NULL) {
    return KHS_NULL;
  }
  if (n_args == 0) {
    return KHS_NULL;
  }
  if (n_args == 1) {
    bool ok = khs_array_push(&ARRAYOF_ARRAY, args[0]);
    if (!ok) {
      return KHS_ERR("Out of memory");
    }
  } 
  else {
    khs_val new_array = khs_new_array(n_args, args, n_args, false);
    if (KHS_TYPEOF(new_array) == KHS_TYPE_NULL) {
      return KHS_ERR("Out of memory");
    }
    bool ok = khs_array_push(&ARRAYOF_ARRAY, new_array);
    khs_release(new_array);
    if (!ok) {
      return KHS_ERR("Out of memory");
    }
  }
  return KHS_NULL;
}

static khs_val khs_arrayof_callback(const khs_val *args, khs_size n_args) {
  static khs_external_fn arrayof_capture_fn = KHS_FN(-1, "arrayof-capture", khs_arrayof_capture_callback);
  khs_val *pass_args = khs_talloc(sizeof(khs_val) * n_args);
  if (pass_args == NULL) {
    return KHS_ERR("Out of memory");
  }
  for (khs_size i = 1; i < n_args; i++) {
    pass_args[i-1] = args[i];
  }
  pass_args[n_args-1] = KHS_EXT_FN(&arrayof_capture_fn);
  khs_val prev_array = ARRAYOF_ARRAY;
  ARRAYOF_ARRAY = khs_new_array(0, NULL, 4, false);
  if (KHS_TYPEOF(ARRAYOF_ARRAY) == KHS_TYPE_NULL) {
    ARRAYOF_ARRAY = prev_array;
    khs_tfree(pass_args);
    return KHS_ERR("Out of memory");
  }
  khs_val res = khs_call(args[0], pass_args, n_args, true);
  khs_tfree(pass_args);
  if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    khs_release(ARRAYOF_ARRAY);
    ARRAYOF_ARRAY = prev_array;
    return res;
  }
  khs_release(res);
  res = ARRAYOF_ARRAY;
  ARRAYOF_ARRAY = prev_array;
  return res;
}

static khs_val khs_construct_array_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (!khs_is_integer(args[0])) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected integer number as first argument of 'construct-array'");
  }
  khs_float f = khs_as_num(args[0]);
  if (f > KHS_SIZE_MAX) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Array would be too large");
  }
  khs_size len = f;
  khs_val array = khs_new_array(0, NULL, len, false);
  if (KHS_TYPEOF(array) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  khs_val *a = (khs_val *) khs_get_raw_array(array);
  for (khs_size i = 0; i < len; i++) {
    khs_val arg = KHS_NUMBER(i);
    khs_val item = khs_call(args[1], &arg, 1, true);
    khs_release(arg);
    if (KHS_TYPEOF(item) == KHS_TYPE_ERR) {
      khs_release(array);
      return item;
    }
    a[i] = item;
  }
  array.len = len;
  return array;
}

static khs_val khs_loop_callback(const khs_val *args, khs_size n_args) {
  (void) n_args;
  while (true) {
    khs_val res = khs_call(args[0], NULL, 0, true);
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR)
      return res;
    if (KHS_TYPEOF(res) != KHS_TYPE_BOOL) {
      khs_blame_arg(res);
      khs_release(res);
      return KHS_ERR("Expected loop body to return boolean");
    }
    if (!khs_as_bool(res)) {
      break;
    }
  }
  return KHS_NULL;
}

static bool khs_match_str(const char *str, const char *str_end, const char *substr, size_t substr_len) {
  assert(str <= str_end);
  size_t str_len = str_end - str;
  if (str_len < substr_len) {
    return false;
  }
  while (substr_len--) {
    assert(str < str_end);
    if (*str != *substr) {
      return false;
    }
    str++;
    substr++;
  }
  return true;
}

static const char *khs_find_str(const char *str, const char *str_end, const char *substr, size_t substr_len) {
  for (const char *c = str; c < str_end; c++) {
    if(khs_match_str(c, str_end, substr, substr_len)) {
      return c;
    }
  }
  return NULL;
}

static const char *find_str_right(const char *str, const char *str_end, const char *substr, size_t substr_len) {
  for (const char *c = str_end - 1; c >= str; c--) {
    if (khs_match_str(c, str_end, substr, substr_len)) {
      return c;
    }
  }
  return NULL;
}

static khs_val khs_find_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE2(KHS_TYPE_STR, "string", KHS_TYPE_STR, "string");
  khs_size str_len = KHS_LENOF(args[0]);
  const char *str = khs_get_raw_str(&args[0]);
  const char *str_end = str + str_len;
  const char *at = khs_find_str(str, str_end, khs_get_raw_str(&args[1]), KHS_LENOF(args[1]));
  if (at == NULL) {
    return KHS_NULL;
  }
  return KHS_NUMBER(at - str);
}

static khs_val khs_beginswith_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE2(KHS_TYPE_STR, "string", KHS_TYPE_STR, "string");
  khs_val res = khs_find_callback(args, n_args);
  return KHS_BOOL(khs_val_cmp(res, KHS_NUMBER(0)) == 0);
}

static khs_val khs_find_right_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE2(KHS_TYPE_STR, "string", KHS_TYPE_STR, "string");
  khs_size str_len = KHS_LENOF(args[0]);
  const char *str = khs_get_raw_str(&args[0]);
  const char *str_end = str + str_len;
  const char *at = find_str_right(str, str_end, khs_get_raw_str(&args[1]), KHS_LENOF(args[1]));
  if (at == NULL) {
    return KHS_NULL;
  }
  return KHS_NUMBER(at - str);
}

static khs_val khs_endswith_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE2(KHS_TYPE_STR, "string", KHS_TYPE_STR, "string");
  khs_size str_len = KHS_LENOF(args[0]);
  const char *str = khs_get_raw_str(&args[0]);
  const char *str_end = str + str_len;
  khs_size substr_len = KHS_LENOF(args[1]);
  const char *at = find_str_right(str, str_end, khs_get_raw_str(&args[1]), substr_len);
  if (at == NULL) {
    return KHS_FALSE;
  }
  return KHS_BOOL(str_end - substr_len == at);
}

static khs_val khs_substring_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE3(KHS_TYPE_STR, "string", KHS_TYPE_NUMBER, "number", KHS_TYPE_NUMBER, "number");
  if (!khs_is_integer(args[1])) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Expected starting index as integer, got '%0'");
  }
  if (!khs_is_integer(args[2])) {
    khs_blame_arg(args[2]);
    return KHS_ERR("Expected ending index as integer, got '%0'");
  }
  khs_float fi_f = khs_as_num(args[1]);
  khs_float ti_f = khs_as_num(args[2]);
  if (fi_f > ti_f) {
    khs_blame_arg(args[1]);
    khs_blame_arg(args[2]);
    return KHS_ERR("Start index (%0) for substring is larger than end index (%1)");
  }
  if (fi_f < 0) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Substring start index (%0) is out of bounds");
  }
  if (ti_f > KHS_SIZE_MAX) {
    khs_blame_arg(args[2]);
    return KHS_ERR("Substring end index (%0) is out of bounds");
  }
  khs_size from = fi_f;
  khs_size to = ti_f;
  khs_size substr_len = to - from;
  if (to > KHS_LENOF(args[0])) {
    khs_blame_arg(args[2]);
    return KHS_ERR("Substring end index (%0) is out of bounds");
  }
  const char *from_str = khs_get_raw_str(&args[0]);
  assert(from_str + from + substr_len <= from_str + KHS_LENOF(args[0]));
  khs_val substr = khs_new_string(substr_len, from_str + from);
  if (KHS_TYPEOF(substr) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  return substr;
}

static khs_val khs_default_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) == KHS_TYPE_NULL) {
    return khs_retain(args[1]);
  }
  else {
    return khs_retain(args[0]);
  }
}

static bool khs_str_buff_init(khs_str_buff *buff) {
  buff->str = khs_alloc(KHS_STR_BUFF_START_SIZE);
  buff->top = buff->str;
  buff->end = buff->str + KHS_STR_BUFF_START_SIZE;
  return buff->str != NULL;
}

static void khs_str_buff_free(khs_str_buff *buff) {
  khs_free(buff->str);
  buff->str = NULL;
  buff->top = NULL;
  buff->end = NULL;
}

static bool khs_str_buff_pushs(khs_str_buff *buff, const char *str, size_t len) {
  char *p = buff->top;
  buff->top += len;
  if (buff->top > buff->end) {
    size_t cap = buff->end - buff->str;
    size_t blen = p - buff->str;
    cap = (cap * 3) / 2 + len;
    char *new_alloc = khs_realloc(buff->str, cap);
    if (new_alloc == NULL) {
      buff->top -= len;
      return false;
    }
    buff->str = new_alloc;
    buff->top = buff->str + blen + len;
    buff->end = buff->str + cap;
    p = buff->str + blen;
  }
  khs_mcpy(p, str, len);
  return true;
}

static khs_val khs_str_replace_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE3(KHS_TYPE_STR, "string", KHS_TYPE_STR, "string", KHS_TYPE_STR, "string");
  const char *str = khs_get_raw_str(&args[0]);
  khs_size str_len = KHS_LENOF(args[0]);
  const char *str_end = str + str_len;
  const char *replace = khs_get_raw_str(&args[1]);
  khs_size replace_len = KHS_LENOF(args[1]);
  const char *replace_with = khs_get_raw_str(&args[2]);
  khs_size replace_with_len = KHS_LENOF(args[2]);
  khs_str_buff buff;
  bool ok = khs_str_buff_init(&buff);
  if (!ok) {
    goto MEM_ERR;
  }
  const char *start = NULL;
  for (const char *c = str; c < str_end; c++) {
    if (khs_match_str(c, str_end, replace, replace_len)) {
      if (start != NULL) {
        bool ok = khs_str_buff_pushs(&buff, start, c - start);
        if (!ok) {
          goto MEM_ERR;  
        }
      }
      bool ok = khs_str_buff_pushs(&buff, replace_with, replace_with_len);
      if (!ok) {
        goto MEM_ERR;
      }
      c += replace_len;
      c--;
      start = NULL;
    } 
    else if(start == NULL) {
      start = c;
    }
  }
  if (start != NULL) {
    bool ok = khs_str_buff_pushs(&buff, start, str_end - start);
    if (!ok) {
      goto MEM_ERR;
    }
  }
  size_t res_len = buff.top - buff.str;
  if (res_len > KHS_SIZE_MAX) {
    khs_str_buff_free(&buff);
    return KHS_ERR("Resulting string would be too large");
  }
  khs_val res = khs_new_string(res_len, buff.str);
  if (KHS_TYPEOF(res) == KHS_TYPE_NULL) {
    goto MEM_ERR;
  }
  khs_str_buff_free(&buff);
  return res;
MEM_ERR:
  khs_str_buff_free(&buff);
  return KHS_ERR("Out of memory");
}

static bool khs_char_is_digit(char c) {
  return c >= '0' && c <= '9';
}

static int khs_char_as_digit(char c) {
  assert(khs_char_is_digit(c));
  return c - '0';
}

static khs_val khs_parse_number_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE1(KHS_TYPE_STR, "string");
  const char *str = khs_get_raw_str(&args[0]);
  khs_size len = KHS_LENOF(args[0]);
  const char *str_end = str + len;
  const char *c = str;
  for (; c != str_end; c++) {
    if (khs_char_is_digit(*c) || *c == '-') {
      break;
    }
  }
  khs_float res = 0;
  bool negative = false;
  if (c != str_end && *c == '-') {
    negative = true;
    c++;
  }
  for (; c != str_end; c++) {
    if (!khs_char_is_digit(*c)) {
      break;
    }
    res *= 10;
    res += khs_char_as_digit(*c);
  }
  if (c != str_end && *c == '.') {
    c++;
    khs_float pow = 1;
    for (; c != str_end; c++) {
      if (!khs_char_is_digit(*c)) {
        break;
      }
      pow *= 0.1;
      res += pow * khs_char_as_digit(*c);
    }
  }
  return KHS_NUMBER(negative ? -res : res);
}

size_t khs_iilog10(size_t i) {
  size_t n = 0;
  while (i /= 10) {
    n++;
  }
  return n;
}

size_t khs_iflog10(khs_float f, khs_float *opt_out) {
  size_t n = 0;
  while ((f /= 10.0) > 1) {
    n++;
  }
  if (opt_out != NULL) {
    *opt_out = f * 10;
  }
  return n;
}

static char khs_int_as_digit(int i) {
  assert(i >= 0 && i <= 9);
  return '0' + i;
}

static void khs_init_stringf(khs_stringf *f, char *str, size_t len) {
  f->str = str;
  f->end = str + len;
  f->at = str;
}

static void khs_write_char(khs_stringf *f, char c) {
  assert(f->at < f->end);
  *(f->at++) = c;
}

static void khs_write_char_at(khs_stringf *f, size_t offset, char c) {
  char *s = f->at + offset;
  assert(s >= f->str && s < f->end);
  *s = c;
}

static void  khs_move_cursor(khs_stringf *f, size_t offset) {
  f->at += offset;
  assert(f->at <= f->end);
}


static khs_val khs_num_to_str(khs_float num) {
  bool negative = num < 0;
  if(negative) {
    num = -num;
  }
  if (num / 10 == num) {
    if (num == 0) {
      return KHS_CONST_STR("0");
    }
    if (negative) {
      return KHS_CONST_STR("-infinity");
    }
    else {
      return KHS_CONST_STR("infinity");
    }
  }
  if (num > ULONG_MAX) {
    khs_float  significand;
    size_t exp = khs_iflog10(num, &significand);
    size_t n_sig_digits = 4;
    size_t n_exp_digits = khs_iilog10(exp) + 1;
    size_t n_total = (negative ? 1 : 0) + (n_sig_digits + 2) + 2 + n_exp_digits;
    khs_val res = khs_new_string(n_total, NULL);
    if (KHS_TYPEOF(res) == KHS_TYPE_NULL) {
      return KHS_NULL;
    }
    khs_stringf s;
    khs_init_stringf(&s, (char *) khs_get_raw_str(&res), n_total);
    if (negative) {
      khs_write_char(&s, '-');
    }
    khs_write_char(&s, khs_int_as_digit(significand));
    khs_write_char(&s, '.');
    for (size_t i = 0; i < n_sig_digits; i++) {
      significand -= (int) significand;
      significand *= 10;
      khs_write_char(&s, khs_int_as_digit(significand));
    }
    khs_write_char(&s, 'e');
    khs_write_char(&s, '+');
    for (size_t i = n_exp_digits - 1; true; i--) {
      khs_write_char_at(&s, i, khs_int_as_digit(exp % 10));
      exp /= 10;
      if (i == 0) {
        break;
      }
    }
    return res;
  }
  unsigned long long int_v = num;
  size_t n_idigits = khs_iilog10(int_v) + 1;
  size_t n_ddigits = khs_is_integer(KHS_NUMBER(num)) ? 0 : 4;
  size_t n_total = (negative ? 1 : 0) + n_idigits + n_ddigits;
  if (n_ddigits != 0) {
    n_total += 1;
  }
  khs_val res = khs_new_string(n_total, NULL);
  if (KHS_TYPEOF(res) == KHS_TYPE_NULL) {
    return KHS_NULL;
  }
  khs_stringf s;
  khs_init_stringf(&s, (char *) khs_get_raw_str(&res), n_total);
  if (negative) {
    khs_write_char(&s, '-');
  }
  unsigned long long i_num = int_v;
  for (size_t i = n_idigits - 1; true; i--) {
    char digit = khs_int_as_digit(i_num % 10);
    i_num /= 10;
    khs_write_char_at(&s, i, digit);
    if (i == 0) {
      break;
    }
  }
  khs_move_cursor(&s, n_idigits);
  if (n_ddigits != 0) {
    khs_write_char(&s, '.');
  }
  khs_float dec_part = num - int_v;
  for (size_t i = 0; i < n_ddigits; i++) {
    dec_part *= 10;
    char digit = khs_int_as_digit(((unsigned long long) dec_part) % 10);
    khs_write_char_at(&s, i, digit);
  }
  return res;
}

khs_val khs_val_as_string(khs_val val) {
  switch (KHS_TYPEOF(val)) {
    case KHS_TYPE_NUMBER: {
      khs_val res = khs_num_to_str(khs_as_num(val));
      if (KHS_TYPEOF(res) == KHS_TYPE_NULL) {
        return KHS_ERR("Out of memory");
      }
      return res; }
    case KHS_TYPE_FN:
    case KHS_TYPE_EXT_FN:
      return KHS_CONST_STR("Function");
    case KHS_TYPE_ARRAY:
      return KHS_CONST_STR("Array");
    case KHS_TYPE_TAG:
      return KHS_CONST_STR("Tag");
    case KHS_TYPE_NULL:
      return KHS_CONST_STR("Null");
    case KHS_TYPE_STR:
      return khs_retain(val);
    case KHS_TYPE_TABLE:
      return KHS_CONST_STR("Table");
    default:
      return KHS_CONST_STR("Unkown");
  }
}

static khs_val khs_as_string_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  return khs_val_as_string(args[0]);
}

static khs_val khs_round_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  KHS_EXPECT_TYPE1(KHS_TYPE_NUMBER, "number");
  khs_float num = khs_as_num(args[0]);
  bool negative = num < 0;
  if (negative) {
    num = -num;
  }
  if (num > KHS_NUM_MAX_INT) {
    return khs_retain(args[0]);
  }
  unsigned long long i = (num + 0.5);
  khs_float res = i;
  if (negative) {
    res = -res;
  }
  return KHS_NUMBER(res);
}

static khs_val khs_is_int_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  return KHS_BOOL(khs_is_integer(args[0]));
}

static khs_val khs_pipe_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  khs_val res = khs_call(args[1], &args[0], 1, false);
  return res;
}

static khs_val khs_error_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected string error message as argument for 'error', got '%0'");
  }
  for (khs_size i = 1; i < n_args; i++) {
    khs_blame_arg(args[i]);  
  }
  return khs_str_as_err(args[0]);
}

static khs_size khs_bt_parent(khs_size i) {
  assert(i != 0);
  return (i - 1) / 2;
}

static khs_size khs_bt_left_child(khs_size i) {
  return i * 2 + 1;
}

static khs_size khs_bt_right_child(khs_size i) {
  return i * 2 + 2;
}

static bool khs_is_inside_heap(khs_size i, khs_size len) {
  return i < len;
}

static inline int khs_checked_cmp(khs_val a, khs_val b) {
  int res = khs_val_cmp(a, b);
  if(res == 2 && !SORT_ERR) {
    SORT_ERR = true;
    khs_blame_arg(a);
    khs_blame_arg(b);
  }
  return res;
}

static void khs_make_heap(khs_val *array, khs_size at, khs_size len) {
START:
  assert(at < len);
  khs_val top = array[at];
  khs_size left_i = khs_bt_left_child(at);
  khs_size right_i = khs_bt_right_child(at);
  if (!khs_is_inside_heap(left_i, len)) {
    assert(!khs_is_inside_heap(right_i, len));
    return;
  }
  khs_val left = array[left_i];
  if (!khs_is_inside_heap(right_i, len)) {
    if (khs_checked_cmp(left, top) == -1) {
      array[at] = left;
      array[left_i] = top;
      at = left_i;
      goto START;
    } 
    else
      return;
  }
  assert(khs_is_inside_heap(right_i, len));
  khs_val right = array[right_i];
  if (khs_checked_cmp(left, top) == -1) {
    if (khs_checked_cmp(left, right) == -1) {
      array[at] = left;
      array[left_i] = top;
      at = left_i;
      goto START;
    } 
    else {
      array[at] = right;
      array[right_i] = top;
      at = right_i;
      goto START;
    }
  }
  if (khs_checked_cmp(right, top) == -1) {
    if (khs_checked_cmp(right, left) == -1) {
      array[at] = right;
      array[right_i] = top;
      at = right_i;
      goto START;
    } 
    else {
      array[at] = left;
      array[left_i] = top;
      at = left_i;
      goto START;
    }
  }
  return;
}

static khs_val khs_sort_array(khs_val *array, khs_size len) {
  for(khs_size i = khs_bt_parent(len - 1); true; i--) {
    khs_make_heap(array, i, len);
    if (i == 0) {
      break;
    }
  }
  for (khs_size heap_len = len; heap_len > 1; heap_len--) {
    khs_val max = array[0];
    khs_size leaf_i = heap_len - 1;
    array[0] = array[leaf_i];
    array[leaf_i] = max;
    khs_make_heap(array, 0, heap_len - 1);
  }
  if (SORT_ERR) {
    SORT_ERR = false;
    return KHS_ERR("Cannot compare values '%0' and '%1'");
  }
  return KHS_NULL;
}

static khs_val khs_sort_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Can only sort arrays");
  }
  khs_val array_to_sort;
  khs_size len = KHS_LENOF(args[0]);
  if (khs_get_refcount(args[0]) == 1) {
    array_to_sort = khs_retain(args[0]);
  } 
  else {
    array_to_sort = khs_new_array(len, khs_get_raw_array(args[0]), len, false); //Creates a copy of the array
  }
  if (KHS_TYPEOF(array_to_sort) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  khs_val err = khs_sort_array((khs_val *) khs_get_raw_array(array_to_sort), len);
  if (KHS_TYPEOF(err) == KHS_TYPE_ERR) {
    khs_release(array_to_sort);
    return err;
  }
  return array_to_sort;
}

static unsigned long long khs_random_from(unsigned long long from) {
  for(size_t i = 0; i < KHS_TAPE_LEN; i++) {
    TAPE[i] *= TAPE[i] + (from+1);
  }
  size_t tape_head = 0;
  unsigned long long res = from;
  for (unsigned long long i = 1; i != 0; i = i << 1) {
    char bit = from & 1;
    from = from >> 1;
    if (bit) {
      tape_head += res + 1;
    }
    else {
      tape_head -= res + 1;
    }
    res *= res;
    res += TAPE[tape_head % KHS_TAPE_LEN];
  }
  return res;
}

static khs_val khs_random_callback(const khs_val *args, khs_size n_args) {
  (void)n_args, (void)args;
  static unsigned long long seed = (unsigned long long)&khs_random_callback;
  seed = khs_random_from(seed);
  return KHS_NUMBER((khs_float) (seed) / (khs_float)ULONG_MAX);
}

static khs_val khs_slice_array(khs_val array, khs_size from, khs_size to) {
  assert(KHS_TYPEOF(array) == KHS_TYPE_ARRAY);
  assert(from <= to);  
  assert(to <= KHS_LENOF(array));
  const khs_val *items = khs_get_raw_array(array);
  if (from == 0 && khs_get_refcount(array) == 1) {
    for (khs_size i = KHS_LENOF(array) - 1; i >= to; i--) {
      khs_release(items[i]);
      if (i == 0) {
        break;
      }
    }
    array.len = to;
    return khs_retain(array);
  }
  khs_size len = to - from;
  assert(from + len <= KHS_LENOF(array));
  khs_val res = khs_new_array(len, items + from, len, false);
  if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    return KHS_ERR("Out of memory");
  }
  return res;
}

static khs_val khs_slice_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE3(KHS_TYPE_ARRAY, "array", KHS_TYPE_NUMBER, "number", KHS_TYPE_NUMBER, "number"); 
  (void)n_args;
  if (!khs_is_integer(args[1])) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Can only use integers as array indicies");
  }
  if (!khs_is_integer(args[2])) {
    khs_blame_arg(args[2]);
    return KHS_ERR("Can only use integers as array indicies");
  }
  khs_float from_n = khs_as_num(args[1]);
  khs_float to_n = khs_as_num(args[2]);
  if (to_n > KHS_SIZE_MAX) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Index out of range");
  }
  if (from_n > to_n) {
    khs_blame_arg(args[1]);
    khs_blame_arg(args[2]);
    return KHS_ERR("'From' index cannot be larger than 'to' index");
  }
  khs_size from = from_n;
  khs_size to = to_n;
  if (from_n < 0) {
    khs_blame_arg(args[1]);
    return KHS_ERR("'From' index out of range");
  }
  if (to > KHS_LENOF(args[0])) {
    khs_blame_arg(args[2]);
    return KHS_ERR("'To' index out of range");
  }
  return khs_slice_array(args[0], from, to);
}

static khs_val khs_pop_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE1(KHS_TYPE_ARRAY, "array"); 
  (void)n_args;
  if (KHS_LENOF(args[0]) == 0) {
    return KHS_ERR("Cannot pop empty array");
  }
  return khs_slice_array(args[0], 0, KHS_LENOF(args[0]) - 1);
}

static khs_val khs_peek_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE1(KHS_TYPE_ARRAY, "array"); (void) n_args;
  khs_size len = KHS_LENOF(args[0]);
  if (len == 0) {
    return KHS_ERR("Cannot peek empty array");
  }
  return khs_retain(khs_get_raw_array(args[0])[len - 1]);
}

static khs_val khs_apply_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[1]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Can only apply arrays to functions");
  }
  return khs_call(args[0], khs_get_raw_array(args[1]), KHS_LENOF(args[1]), true);
}

static khs_val khs_join_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE2(KHS_TYPE_ARRAY, "array", KHS_TYPE_ARRAY, "array");
  khs_size alen = KHS_LENOF(args[0]);
  khs_size blen = KHS_LENOF(args[1]);
  if (blen > KHS_SIZE_MAX - alen) {
    return KHS_ERR("Out of memory");
  }
  khs_size res_len = alen + blen;
  khs_val res_array = khs_new_array(res_len, NULL, res_len, false);
  if (KHS_TYPEOF(res_array) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  const khs_val *a_a = khs_get_raw_array(args[0]);
  const khs_val *b_a = khs_get_raw_array(args[1]);
  khs_val *res_a = (khs_val *) khs_get_raw_array(res_array);
  while (alen--) {
    *(res_a++) = khs_retain(*(a_a++));
  }
  while (blen--) {
    *(res_a++) = khs_retain(*(b_a++));
  }
  return res_array;
}

static khs_val khs_map_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Can only 'map' arrays");
  }
  khs_size len = KHS_LENOF(args[0]);
  khs_val map_array;
  if (khs_get_refcount(args[0]) == 1) {
    map_array = khs_retain(args[0]);
  } 
  else {
    map_array = khs_new_array(len, NULL, len, 0);
    if (KHS_TYPEOF(map_array) == KHS_TYPE_NULL) {
      return KHS_ERR("Out of memory");
    }
    khs_val *a = (khs_val *) khs_get_raw_array(map_array);
    for (khs_size i = 0; i < len; i++) {
      a[i] = KHS_NULL;
    }
  }
  assert(KHS_TYPEOF(map_array) == KHS_TYPE_ARRAY);
  assert(KHS_LENOF(map_array) >= len);
  khs_val *dst = (khs_val *) khs_get_raw_array(map_array);
  const khs_val *src = khs_get_raw_array(args[0]);
  for (khs_size i = 0; i < len; i++) {
    khs_val arg = khs_retain(src[i]);
    khs_release(dst[i]);
    khs_val res = khs_call(khs_retain(args[1]), &arg, 1, false);
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
      khs_release(map_array);
      return res;
    }
    dst[i] = res;
  }
  return map_array;
}

static khs_val khs_forevery_callback(const khs_val *args, khs_size n_args) {
  khs_val res = KHS_NULL;
  khs_val fn = args[n_args - 1];
  assert(n_args > 0);
  for (khs_size i = 0; i < n_args - 1; i++) {
    khs_release(res);
    res = khs_call(khs_retain(fn), &args[i], 1, false);
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
      for (khs_size j = i + 1; j < n_args - 1; j++) {
        khs_release(args[i]);
      }
      khs_release(fn);
      return res;
    }
  }
  khs_release(fn);
  return res;
}

static khs_val khs_strip_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE1(KHS_TYPE_STR, "string");
  const char *str = khs_get_raw_str(&args[0]);
  khs_size len = KHS_LENOF(args[0]);
  const char *str_end = str + len;
  const char *start;
  for (start = str; start < str_end; start++) {
    if (*start != '\t' && *start != ' ' && *start != '\r' && *start != '\n') {
      break;
    }
  }
  const char *end;
  for (end = str_end - 1; end >= start; end--) {
    if (*end != '\t' && *end != ' ' && *end != '\r' && *end != '\n') {
      break;
    }
  }
  khs_val new_str = khs_new_string((end - start) + 1, start);
  if(KHS_TYPEOF(new_str) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  return new_str;
}

static khs_val khs_split_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE2(KHS_TYPE_STR, "string", KHS_TYPE_STR, "string");
  const char *str = khs_get_raw_str(&args[0]);
  khs_size len = KHS_LENOF(args[0]);
  const char *str_end = str + len;
  const char *split_at = khs_get_raw_str(&args[1]);
  khs_size split_at_len = KHS_LENOF(args[1]);
  if (split_at_len == 0) {
    return KHS_ERR("String splitter cannot be an empty string");
  }
  khs_size tmp_buff_cap = 4, tmp_buff_len = 0;
  khs_val *tmp_buff = khs_alloc(tmp_buff_cap * sizeof(khs_val));
  if (tmp_buff == NULL) {
    return KHS_ERR("Out of memory");
  }
  const char *prev = NULL;
  for (const char *c = str; c < str_end;) {
    if (khs_match_str(c, str_end, split_at, split_at_len)) {
      khs_val new_str;
      if (prev == NULL) {
        new_str = KHS_CONST_STR("");
      }
      else {
        new_str = khs_new_string(c - prev, prev);
      }
      if (KHS_TYPEOF(new_str) == KHS_TYPE_NULL) {
        goto MEM_ERR;
      }
      if (tmp_buff_len == tmp_buff_cap) {
        tmp_buff_cap *= 2;
        if (tmp_buff_cap <= tmp_buff_len) {
          khs_release(new_str);
          goto MEM_ERR;
        }
        khs_val *tmp = khs_realloc(tmp_buff, tmp_buff_cap * sizeof(khs_val));
        if (tmp == NULL) {
          khs_release(new_str);
          goto MEM_ERR;
        }
        tmp_buff = tmp;
      }
      tmp_buff[tmp_buff_len++] = new_str;
      prev = NULL;
      c += split_at_len;
    } 
    else {
      if (prev == NULL) {
        prev = c;
      }
      c++;
    }
  }
  khs_val new_str;
  if (prev == NULL) {
    new_str = KHS_CONST_STR("");
  }
  else {
    new_str = khs_new_string(str_end - prev, prev);
  }
  if (KHS_TYPEOF(new_str) == KHS_TYPE_NULL) {
    goto MEM_ERR;
  }
  if (tmp_buff_len == tmp_buff_cap) {
    tmp_buff_cap += 1;
    if (tmp_buff_cap <= tmp_buff_len) {
      khs_release(new_str);
      goto MEM_ERR;
    }
    khs_val *tmp = khs_realloc(tmp_buff, tmp_buff_cap * sizeof(khs_val));
    if (tmp == NULL) {
      khs_release(new_str);
      goto MEM_ERR;
    }
    tmp_buff = tmp;
  }
  tmp_buff[tmp_buff_len++] = new_str;
  khs_val res_array = khs_new_array(tmp_buff_len, tmp_buff, tmp_buff_len, false);
  for (khs_size i = 0; i < tmp_buff_len; i++) {
    khs_release(tmp_buff[i]);
  }
  khs_free(tmp_buff);
  if (KHS_TYPEOF(res_array) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  return res_array;
MEM_ERR:
  for (khs_size i = 0; i < tmp_buff_len; i++) {
    khs_release(tmp_buff[i]);
  }
  khs_free(tmp_buff);
  return KHS_ERR("Out of memory");
}

static khs_val khs_identity_callback(const khs_val *args, khs_size n_args) {
  return khs_retain(args[0]);
}

static khs_val khs_typeof_callback(const khs_val *args, khs_size n_args) {
  switch (KHS_TYPEOF(args[0])) {
    case KHS_TYPE_NUMBER:
      return KHS_CONST_STR("number");
    case KHS_TYPE_TABLE:
      return KHS_CONST_STR("struct");
    case KHS_TYPE_ARRAY:
      return KHS_CONST_STR("array");
    case KHS_TYPE_NULL:
      return KHS_CONST_STR("null");
    case KHS_TYPE_STR:
      return KHS_CONST_STR("string");
    case KHS_TYPE_FN:
    case KHS_TYPE_EXT_FN:
      return KHS_CONST_STR("function");
    case KHS_TYPE_BOOL:
      return KHS_CONST_STR("bool");
    case KHS_TYPE_TAG:
      return KHS_CONST_STR("tag");
    case KHS_TYPE_OBJECT:
      return KHS_CONST_STR("object");
    default:
      assert(false);
      return KHS_CONST_STR("unkown");
  }
}

static khs_val khs_join_with_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE2(KHS_TYPE_ARRAY, "array", KHS_TYPE_STR, "string");
  khs_size items_total_len = 0;
  const khs_val *items = khs_get_raw_array(args[0]);
  khs_size n_items = KHS_LENOF(args[0]);
  if (n_items == 0) {
    return KHS_CONST_STR("");
  }
  for (khs_size i = 0; i < n_items; i++) {
    if (KHS_TYPEOF(items[i]) != KHS_TYPE_STR) {
      khs_blame_arg(items[i]);
      return KHS_ERR("Array passed to 'join-with' must contain strings only");
    }
    khs_size len = KHS_LENOF(items[i]);
    if (len > KHS_SIZE_MAX - items_total_len) {
      return KHS_ERR("Resulting string would be too large");
    }
    items_total_len += len;
  }
  khs_val join_str = args[1];
  assert(n_items >= 1);
  khs_size n_joiners = n_items - 1;
  if (KHS_LENOF(join_str) != 0 && n_joiners > KHS_SIZE_MAX / KHS_LENOF(join_str)) {
    return KHS_ERR("Resulting string would be too large");
  }
  khs_size total_joiners_len = n_joiners * KHS_LENOF(join_str);
  if (total_joiners_len > KHS_SIZE_MAX - items_total_len) {
    return KHS_ERR("Resulting string would be too large");
  }
  khs_size total_len = items_total_len + total_joiners_len;
  khs_val new_str = khs_new_string(total_len, NULL);
  if (KHS_TYPEOF(new_str) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  char *str = (char *) khs_get_raw_str(&new_str);
  const char *str_end = str + total_len;
  for (khs_size i = 0; i < n_items; i++) {
    khs_val item = items[i];
    khs_size len = KHS_LENOF(item);
    khs_mcpy(str, khs_get_raw_str(&item), len);
    str += len;
    assert(str <= str_end);
    if (i != n_items - 1) {
      khs_mcpy(str, khs_get_raw_str(&join_str), KHS_LENOF(join_str));
      str += KHS_LENOF(join_str);
      assert(str <= str_end);
    }
  }
  return new_str;
}

static khs_val khs_repeat_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE2(KHS_TYPE_STR, "string", KHS_TYPE_NUMBER, "number");
  if (!khs_is_integer(args[1])) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Can only repeat strings an integer number of times");
  }
  khs_float num = khs_as_num(args[1]);
  if (num < 0 || num > KHS_SIZE_MAX) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Invalid number of repetitions");
  }
  khs_size len = KHS_LENOF(args[0]);
  khs_size times = num;
  if (len == 0 || times == 0) {
    return KHS_CONST_STR("");
  }
  if (times > KHS_SIZE_MAX / len) {
    return KHS_ERR("Resulting string would be too large");
  }
  khs_size total_len = times * len;
  khs_val str_res = khs_new_string(total_len, NULL);
  if(KHS_TYPEOF(str_res) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  const char *src_str = khs_get_raw_str(&args[0]);
  char *str = (char *)khs_get_raw_str(&str_res);
  const char *str_end = str + total_len;
  for (khs_size i = 0; i < times; i++) {
    khs_mcpy(str, src_str, len);
    str += len;
    assert(str <= str_end);
  }
  return str_res;
}

static khs_val khs_max_callback(const khs_val *args, khs_size n_args) {
  khs_val max = args[0];
  for (khs_size i = 1; i < n_args; i++) {
    int cmp = khs_val_cmp(args[i], max);
    if (cmp == 2) {
      khs_blame_arg(max); khs_blame_arg(args[i]);
      return KHS_ERR("Uncomparable values");
    }
    if (cmp == -1) {
      max = args[i];
    }
  }
  return khs_retain(max);
}

static khs_val khs_find_in_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected array as first argument for 'find-in'");
  }
  const khs_val *a = khs_get_raw_array(args[0]);
  khs_size len = KHS_LENOF(args[0]);
  for (khs_size i = 0; i < len; i++) {
    if (khs_val_cmp(a[i], args[1]) == 0) {
      return KHS_NUMBER(i);
    }
  }
  return KHS_NULL;
}

static khs_val khs_floor_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE1(KHS_TYPE_NUMBER, "number");
  if (khs_is_integer(args[0])) {
    return khs_retain(args[0]);
  }
  return KHS_NUMBER((unsigned long long)khs_as_num(args[0]));
}

static khs_val khs_ceil_callback(const khs_val *args, khs_size n_args) {
  KHS_EXPECT_TYPE1(KHS_TYPE_NUMBER, "number");
  if (khs_is_integer(args[0])) {
    return khs_retain(args[0]);
  }
  khs_float res = (unsigned long long)khs_as_num(args[0]);
  res += 1;
  return KHS_NUMBER(res);
}

static khs_val khs_first_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected array as first argument");
  }
  const khs_val *a = khs_get_raw_array(args[0]);
  for (khs_size i = 0; i < KHS_LENOF(args[0]); i++) {
    khs_val res = khs_call(args[1], &a[i], 1, true);
    if (KHS_TYPEOF(res) != KHS_TYPE_BOOL) {
      khs_blame_arg(res);
      khs_release(res);
      return KHS_ERR("Expected boolean as result from callback function, got '%0'");
    }
    if (khs_as_bool(res)) {
      return KHS_NUMBER(i);
    }
  }
  return KHS_NULL;
}

static khs_val khs_exists_callback(const khs_val *args, khs_size n_args) {
  khs_val res = khs_first_callback(args, n_args);
  if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
    return res;
  }
  return KHS_BOOL(KHS_TYPEOF(res) != KHS_TYPE_NULL);
}

static khs_val khs_all_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_ARRAY) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected array as first argument");
  }
  const khs_val *a = khs_get_raw_array(args[0]);
  for (khs_size i = 0; i < KHS_LENOF(args[0]); i++) {
    khs_val res = khs_call(args[1], &a[i], 1, true);
    if (KHS_TYPEOF(res) == KHS_TYPE_ERR) {
      return res;
    }
    if (KHS_TYPEOF(res) != KHS_TYPE_BOOL) {
      khs_blame_arg(res);
      khs_release(res);
      return KHS_ERR("Expected boolean as result from callback function, got '%0'");
    }
    if (!khs_as_bool(res)) {
      return KHS_FALSE;
    }
  }
  return KHS_TRUE;
}

static size_t khs_strlen(const char *str) {
  size_t l = 0;
  while (*str != '\0') {
    str++;
    l++;
  }
  return l;
}

bool khs_load_core_lib() {
  static khs_external_fn fns[] = {
    KHS_FN(-3, "+", khs_add_callback),
    KHS_FN(-2, "-", khs_sub_callback),
    KHS_FN(-3, "*", khs_mul_callback),
    KHS_FN(-3, "/", khs_div_callback),
    KHS_FN(-3, "if", khs_if_callback),
    KHS_FN(2, "==", khs_eq_callback),
    KHS_FN(2, "=/=", khs_not_eq_callback),
    KHS_FN(1, "not", khs_not_callback),
    KHS_FN(-1, "array", khs_array_callback),
    KHS_FN(2, "for-in", khs_for_in_callback),
    KHS_FN(3, "for", khs_for_callback),
    KHS_FN(2, "foreach-in", khs_foreach_in_callback),
    KHS_FN(-1, "table", khs_table_callback),
    KHS_FN(-1, "struct", khs_table_callback),
    KHS_FN(0, "tag", khs_tag_callback),
    KHS_MANUAL_RELEASE_FN(1, "invoke", khs_invoke_callback),
    KHS_FN(1, "new", khs_invoke_callback),
    KHS_FN(-2, "assert", khs_assert_callback),
    KHS_FN(-3, "and?", khs_and_callback),
    KHS_FN(-3, "or?", khs_or_callback),
    KHS_FN(2, "<", khs_less_callback),
    KHS_FN(2, ">", khs_greater_callback), 
    KHS_FN(2, "=<=", khs_less_eq_callback),
    KHS_FN(2, "=>=", khs_greater_eq_callback),
    KHS_FN(3, "insert", khs_insert_callback),
    KHS_FN(2, "union", khs_union_callback),
    KHS_FN(-3, "cat", khs_concat_callback),
    KHS_MANUAL_RELEASE_FN(2, "in?", khs_in_callback),
    KHS_FN(1, "sizeof", khs_sizeof_callback),
    KHS_FN(3, "replace", khs_replace_callback),
    KHS_FN(-2, "eval", khs_eval_callback),
    KHS_MANUAL_RELEASE_FN(2, "filter", khs_filter_callback),
    KHS_FN(2, "push", khs_push_callback),
    KHS_FN(2, "mod", khs_mod_callback),
    KHS_FN(-2, "arrayof", khs_arrayof_callback),
    KHS_FN(2, "construct-array", khs_construct_array_callback),
    KHS_FN(1, "loop", khs_loop_callback),
    KHS_FN(2, "find", khs_find_callback),
    KHS_FN(2, "beginswith?", khs_beginswith_callback),
    KHS_FN(2, "find-right", khs_find_right_callback),
    KHS_FN(2, "endswith?", khs_endswith_callback),
    KHS_FN(3, "substring", khs_substring_callback),
    KHS_FN(2, "default", khs_default_callback),
    KHS_FN(3, "str-replace", khs_str_replace_callback),
    KHS_FN(1, "parse-number", khs_parse_number_callback),
    KHS_FN(1, "as-string", khs_as_string_callback),
    KHS_FN(1, "round", khs_round_callback),
    KHS_FN(1, "is-int", khs_is_int_callback),
    KHS_MANUAL_RELEASE_FN(2, "->", khs_pipe_callback),
    KHS_FN(3, "try", khs_try_callback),
    KHS_FN(-2, "error", khs_error_callback),
    KHS_FN(1, "sort", khs_sort_callback),
    KHS_FN(0, "random", khs_random_callback),
    KHS_FN(3, "slice", khs_slice_callback),
    KHS_FN(1, "pop", khs_pop_callback),
    KHS_FN(1, "peek", khs_peek_callback),
    KHS_FN(2, "apply", khs_apply_callback),
    KHS_FN(2, "join", khs_join_callback),
    KHS_FN(2, "map", khs_map_callback),
    KHS_MANUAL_RELEASE_FN(-3, "forevery", khs_forevery_callback),
    KHS_FN(1, "strip", khs_strip_callback),
    KHS_FN(2, "split", khs_split_callback),
    KHS_FN(1, "identity", khs_identity_callback),
    KHS_FN(1, "typeof", khs_typeof_callback),
    KHS_FN(2, "join-with", khs_join_with_callback),
    KHS_FN(2, "repeat", khs_repeat_callback),
    KHS_FN(-2, "max", khs_max_callback),
    KHS_FN(2, "find-in", khs_find_in_callback),
    KHS_FN(1, "floor", khs_floor_callback),
    KHS_FN(1, "ceil", khs_ceil_callback),
    KHS_FN(2, "first", khs_first_callback),
    KHS_FN(2, "exists", khs_exists_callback),
    KHS_FN(2, "all", khs_all_callback)
  };
  ELSE_TAG = khs_new_tag();
  ELSEIF_TAG = khs_new_tag();
  CATCH_TAG = khs_new_tag();
  PRINT_TRACE_TAG = khs_new_tag();
  khs_set_var("else", sizeof("else") - 1, ELSE_TAG, true);
  khs_set_var("elseif", sizeof("elseif") - 1, ELSEIF_TAG, true);
  khs_set_var("catch", sizeof("catch") - 1, CATCH_TAG, true);
  khs_set_var("catch-log", sizeof("catch-log") - 1, PRINT_TRACE_TAG, true);
  for(size_t i = 0; i < KHS_UTIL_LENOF(fns); i++) {
    bool ok = khs_set_var(fns[i].name, fns[i].name_len, KHS_EXT_FN(&fns[i]), true);
    if (!ok) {
      return false;
    }
  }
  khs_set_var("false", sizeof("false") - 1, KHS_FALSE, true);
  khs_set_var("true", sizeof("true") - 1, KHS_TRUE, true);
  khs_set_var("null", sizeof("null") - 1, KHS_NULL, true);
  khs_set_var("max-int", sizeof("max-int") - 1, KHS_NUMBER(KHS_NUM_MAX_INT), true);
  khs_set_var("quote", sizeof("quote") - 1, KHS_CONST_STR("\""), true);
  khs_set_var("newline", sizeof("newline") - 1, KHS_CONST_STR("\n"), true);
  khs_set_var("tab", sizeof("tab") - 1, KHS_CONST_STR("\t"), true);
  khs_set_var("carriage-return", sizeof("carriage-return") - 1, KHS_CONST_STR("\r"), true);
  KHS_DEF_FN("pairs", t, arrayof foreach-in t);
  KHS_DEF_FN("implies?", x y, (not x) or? y);
  KHS_DEF_FN("empty?", container, (sizeof container) == 0);
  return true;
}

