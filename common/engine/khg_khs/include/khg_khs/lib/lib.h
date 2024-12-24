#pragma once

#include "khg_khs/khs.h"
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#define KHS_PATH_SEPARATOR "\\"
#else
#define KHS_PATH_SEPARATOR "/"
#endif

#define KHS_FN(arity, name, fn) { arity, true, name, sizeof(name) - 1, fn }
#define KHS_MANUAL_RELEASE_FN(arity, name, fn) { arity, false, name, sizeof(name) - 1, fn }

#define KHS_REQ_NUM(name, strname)\
  khs_val name = khs_call(args[0], &KHS_CONST_STR(strname), 1, true);\
  if (KHS_TYPEOF(name) == KHS_TYPE_ERR)\
    return name;\
  if (KHS_TYPEOF(name) != KHS_TYPE_NUMBER || !khs_is_integer(name)) {\
    khs_blame_arg(args[0]);\
    khs_blame_arg(name);\
    khs_release(name);\
    return KHS_ERR("Expected integer number given '" #strname "' for %0, got %1");\
  }

#define KHS_POP_ARG(to, err_msg) { if(n_args > 0) { n_args--; to = *args; args++; } else { return KHS_ERR("Unexpected end of arguments; " err_msg); } }
#define KHS_EXPECT_TYPE(val, type, err_msg) { if(KHS_TYPEOF(val) != type) { khs_blame_arg(val); return KHS_ERR(err_msg); } }

#define KHS_MATH_OP(name, start_val, start_index, op)\
static khs_val name(const khs_val *args, khs_size n_args) {\
  for (int i = 0; i < start_index; i++) {\
    if (KHS_TYPEOF(args[i]) != KHS_TYPE_NUMBER) {\
      khs_blame_arg(args[i]);\
      return KHS_ERR("Expected number as argument for '" #op "'");\
    }\
  }\
  khs_float res = start_val;\
  for (khs_size i = start_index; i < n_args; i++) {\
    if(KHS_TYPEOF(args[i]) != KHS_TYPE_NUMBER) {\
      khs_blame_arg(args[i]);\
      return KHS_ERR("Expected number as argument for '" #op "'");\
    }\
    res = res op khs_as_num(args[i]);\
  }\
  return KHS_NUMBER(res);\
}

#define KHS_BOOL_OP(name, display_name, op)\
khs_val name(const khs_val *args, khs_size n_args) {\
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_BOOL) {\
    khs_blame_arg(args[0]);\
    return KHS_ERR("Expected boolean as argument for '" display_name "'");\
  }\
  bool res = khs_as_bool(args[0]);\
  for (khs_size i = 1; i < n_args; i++) {\
    if (KHS_TYPEOF(args[i]) != KHS_TYPE_BOOL) {\
      khs_blame_arg(args[i]);\
      return KHS_ERR("Expected boolean as argument for '" display_name "'");\
    }\
    res = res op khs_as_bool(args[i]);\
  }\
  return KHS_BOOL(res);\
}

#define KHS_CMP_OP(name, ...)\
khs_val name(const khs_val *args, khs_size n_args) {\
  khs_val prev = args[0];\
  for (khs_size i = 1; i < n_args; i++) {\
    khs_val next = args[i];\
    { __VA_ARGS__ }\
    prev = next;\
  }\
  return KHS_TRUE;\
}

bool khs_load_core_lib();
khs_val khs_val_as_string(khs_val val);
void khs_core_lib_clear_evals();

bool khs_load_debug_lib();
bool khs_load_io_lib();
bool khs_load_unix_lib();

