#pragma once

#include "khg_khs/khs.h"
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#define FN(arity, name, fn) { arity, true, name, sizeof(name) - 1, fn }
#define MANUAL_RELEASE_FN(arity, name, fn) { arity, false, name, sizeof(name) - 1, fn }

#define REQ_NUM(name, strname)\
  i_val name = beryl_call(args[0], &BERYL_CONST_STR(strname), 1, true);\
  if (BERYL_TYPEOF(name) == TYPE_ERR)\
    return name;\
  if (BERYL_TYPEOF(name) != TYPE_NUMBER || !beryl_is_integer(name)) {\
    beryl_blame_arg(args[0]);\
    beryl_blame_arg(name);\
    beryl_release(name);\
    return BERYL_ERR("Expected integer number given '" #strname "' for %0, got %1");\
  }

#define POP_ARG(to, err_msg) { if(n_args > 0) { n_args--; to = *args; args++; } else { return BERYL_ERR("Unexpected end of arguments; " err_msg); } }
#define EXPECT_TYPE(val, type, err_msg) { if(BERYL_TYPEOF(val) != type) { beryl_blame_arg(val); return BERYL_ERR(err_msg); } }

#define MATH_OP(name, start_val, start_index, op)\
static i_val name(const i_val *args, i_size n_args) {\
	for (int i = 0; i < start_index; i++) {\
		if (BERYL_TYPEOF(args[i]) != TYPE_NUMBER) {\
			beryl_blame_arg(args[i]);\
			return BERYL_ERR("Expected number as argument for '" #op "'");\
		}\
	}\
	i_float res = start_val;\
	for (i_size i = start_index; i < n_args; i++) {\
		if(BERYL_TYPEOF(args[i]) != TYPE_NUMBER) {\
			beryl_blame_arg(args[i]);\
			return BERYL_ERR("Expected number as argument for '" #op "'");\
		}\
		res = res op beryl_as_num(args[i]);\
	}\
	return BERYL_NUMBER(res);\
}

#define BOOL_OP(name, display_name, op)\
static i_val name(const i_val *args, i_size n_args) {\
	if (BERYL_TYPEOF(args[0]) != TYPE_BOOL) {\
		beryl_blame_arg(args[0]);\
		return BERYL_ERR("Expected boolean as argument for '" display_name "'");\
	}\
	bool res = beryl_as_bool(args[0]);\
	for (i_size i = 1; i < n_args; i++) {\
		if (BERYL_TYPEOF(args[i]) != TYPE_BOOL) {\
			beryl_blame_arg(args[i]);\
			return BERYL_ERR("Expected boolean as argument for '" display_name "'");\
		}\
		res = res op beryl_as_bool(args[i]);\
	}\
	return BERYL_BOOL(res);\
}

#define CMP_OP(name, ...)\
static i_val name(const i_val *args, i_size n_args) {\
	i_val prev = args[0];\
	for (i_size i = 1; i < n_args; i++) {\
		i_val next = args[i];\
		{ __VA_ARGS__ }\
		prev = next;\
	}\
	return BERYL_TRUE;\
}

bool load_core_lib();
i_val i_val_as_string(i_val val);
void beryl_core_lib_clear_evals();

bool load_debug_lib();
bool load_io_lib();
bool load_unix_lib();

