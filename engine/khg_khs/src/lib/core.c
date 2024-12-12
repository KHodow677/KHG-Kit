#include "khg_khs/khs.h"
#include "khg_khs/lib/lib.h"
#include "khg_khs/util.h"
#include <assert.h>

#define EXPECT_TYPE_I(index, type, name, ordinal_name) {\
	assert(n_args > index);\
	if(BERYL_TYPEOF(args[index]) != type) {\
		beryl_blame_arg(args[index]);\
		return BERYL_ERR("Expected " name " as " ordinal_name " argument, got '%0'");\
	}\
}

#define EXPECT_TYPE1(t1, name1)\
{\
	EXPECT_TYPE_I(0, t1, name1, "first");\
}

#define EXPECT_TYPE2(t1, name1, t2, name2)\
{\
	EXPECT_TYPE1(t1, name1);\
	EXPECT_TYPE_I(1, t2, name2, "second");\
}

#define EXPECT_TYPE3(t1, name1, t2, name2, t3, name3) {\
	EXPECT_TYPE2(t1, name1, t2, name2);\
	EXPECT_TYPE_I(2, t3, name3, "third");\
}

#define DEF_FN(name, args, ...) beryl_set_var(name, sizeof(name) - 1, BERYL_LITERAL_FN(args, __VA_ARGS__), true)

#define TAPE_LEN (sizeof(tape) / sizeof(tape[0]))
#define LSTR(str) str, sizeof(str) - 1

#define STR_BUFF_START_SIZE 16

typedef struct str_buff {
	char *str, *top, *end;
} str_buff;

typedef struct stringf {
	char *str, *end, *at;
} stringf;

static i_val else_tag, elseif_tag;

typedef struct eval_item eval_item;
static struct eval_item {
	eval_item *prev;
	i_val item;
} *eval_list = NULL;

static i_val catch_tag;
static i_val print_trace_tag;

static i_val arrayof_array = BERYL_NULL;

static bool sort_err = false;

static khs_large_uint_type tape[] = { 59243892, 2014914089654949231, 120301499583420, 23, 3230239239, 120302102103, 904924490212, 10412  };

static void mcpy(void *to_ptr, const void *from_ptr, size_t n) {
	char *to = to_ptr;
	const char *from = from_ptr;
	while(n--) {
		*(to++) = *(from++);
  }
}

KHS_MATH_OP(add_callback, 0, 0, +) 
KHS_MATH_OP(mul_callback, 1, 0, *)
KHS_MATH_OP(div_callback, beryl_as_num(args[0]), 1, /)

static i_val sub_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[0]) != TYPE_NUMBER) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected number as first argument for '-'");
	}
	khs_float res = beryl_as_num(args[0]);
	if (n_args == 1) {
		return BERYL_NUMBER(-res);
  }
	for (khs_size i = 1; i < n_args; i++) {
		if(BERYL_TYPEOF(args[i]) != TYPE_NUMBER) {
			beryl_blame_arg(args[i]);
			return BERYL_ERR("Expected number as argument for '-'");
		}
		res -= beryl_as_num(args[i]);
	}
	return BERYL_NUMBER(res);
}

static i_val if_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[0]) != TYPE_BOOL) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected boolean as if condition");
	}
	if (beryl_as_bool(args[0])) {
		return beryl_call(args[1], NULL, 0, true);
	}
	assert(n_args >= 2);
	n_args -= 2;
	args += 2;
	while (n_args > 0) {
		i_val tag;
		KHS_POP_ARG(tag, "");
    if (beryl_val_cmp(tag, elseif_tag) == 0) {
			i_val cond;
			KHS_POP_ARG(cond, "Expected condition following 'elseif'");
			KHS_EXPECT_TYPE(cond, TYPE_BOOL, "Expected boolean condition following 'elseif'");
			i_val then_do;
			KHS_POP_ARG(then_do, "Expected argument following 'elseif'");
		  if (beryl_as_bool(cond)) {
				return beryl_call(then_do, NULL, 0, true);
      }
		} 
    else if (beryl_val_cmp(tag, else_tag) == 0) {
			i_val then_do;
			KHS_POP_ARG(then_do, "Expected argument following 'else'");
			return beryl_call(then_do, NULL, 0, true);
		} 
    else {
			beryl_blame_arg(tag);
			return BERYL_ERR("Unexpected argument for 'if'. Expected either 'elseif' or 'else'");
		}
	}
	if (n_args != 0) {
		i_val arg;
		KHS_POP_ARG(arg, "");
		beryl_blame_arg(arg);
		return BERYL_ERR("Expected no more arguments after 'else'");
	}
	return BERYL_NULL;
}

static i_val eq_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	return BERYL_BOOL(beryl_val_cmp(args[0], args[1]) == 0);
}

static i_val not_eq_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	return BERYL_BOOL(beryl_val_cmp(args[0], args[1]) != 0);
}

static i_val not_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) != TYPE_BOOL) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected boolean as argument to 'not'");
	}
	return BERYL_BOOL(!beryl_as_bool(args[0]));
}

static i_val array_callback(const i_val *args, khs_size n_args) {
	i_val array = beryl_new_array(n_args, args, n_args, false);
	if (BERYL_TYPEOF(array) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	return array;
}

static i_val for_in_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	i_val index = BERYL_NUMBER(0);
	i_val res = BERYL_NULL;
	while (true) {
		i_val index_res = beryl_call(args[0], &index, 1, true);
		if (BERYL_TYPEOF(index_res) == TYPE_ERR) {
			return index_res;
    }
		if (BERYL_TYPEOF(index_res) == TYPE_NULL) {
			break;
    }
		beryl_release(res);
		res = beryl_call(beryl_retain(args[1]), &index_res, 1, false);
		if (BERYL_TYPEOF(res) == TYPE_ERR) {
			return res;
    }
		index = BERYL_NUMBER(beryl_as_num(index) + 1);
	}
	return res;
}

static i_val for_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) != TYPE_NUMBER) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("First argument of 'for' must be number");
	}
	if (BERYL_TYPEOF(args[1]) != TYPE_NUMBER) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Second argument of 'for' must be number");
	}
	khs_float from = beryl_as_num(args[0]);
	khs_float until = beryl_as_num(args[1]);
	khs_float i = from;
	i_val res = BERYL_NULL;
  if (from <= until) {
		while (i < until) {
			i_val index = BERYL_NUMBER(i++);
			beryl_release(res);
			res = beryl_call(args[2], &index, 1, true);
			beryl_release(index);
			if (BERYL_TYPEOF(res) == TYPE_ERR) {
				return res;
      }
		}
	} 
  else {
		while (i > until) {
			i_val index = BERYL_NUMBER(i--);
			beryl_release(res);
			res = beryl_call(args[2], &index, 1, true);
			beryl_release(index);
			if (BERYL_TYPEOF(res) == TYPE_ERR) {
				return res;
      }
		}
	}
	return res;
}

static i_val table_callback(const i_val *args, khs_size n_args) {
	if (n_args % 2 != 0) {
		return BERYL_ERR("Table function only accepts an even number of arguments");
  }
	i_val table = beryl_new_table(n_args / 2, true);
	if (BERYL_TYPEOF(table) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	for (khs_size i = 0; i < n_args; i += 2) {
		assert(i + 1 < n_args);
		int res = beryl_table_insert(&table, args[i], args[i + 1], false);
		if (res != 0) {
			beryl_blame_arg(args[i]);
			beryl_release(table);
			switch (res) {
				case 3: 
					return BERYL_ERR("Value is not valid key");
				case 2:
					return BERYL_ERR("Duplicate key");
				default:
					assert(false);
			}
		}
	}
	return table;
}

static i_val tag_callback(const i_val *args, khs_size n_args) {
	(void)n_args, (void)args;
	i_val tag = beryl_new_tag();
	if (BERYL_TYPEOF(tag) == TYPE_NULL) {
		return BERYL_ERR("Out of tags");
  }
	return tag;
}

static i_val invoke_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	i_val res = beryl_call(args[0], NULL, 0, false);
	return res;
}

static i_val assert_callback(const i_val *args, khs_size n_args) {
	if (n_args != 1 && n_args != 2) {
		return BERYL_ERR("Assert takes either one or two arguments");
  }
	if (n_args == 2 && BERYL_TYPEOF(args[1]) != TYPE_STR) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Expected string message as second argument for 'assert'");
	}
	bool passed_assertion;
	if (BERYL_TYPEOF(args[0]) == TYPE_BOOL) {
		passed_assertion = beryl_as_bool(args[0]);
  }
	else {
		passed_assertion = BERYL_TYPEOF(args[0]) != TYPE_NULL;
  }
	if (!passed_assertion) {
		if (n_args == 1) {
			return BERYL_ERR("Assertion failed");
    }
		return beryl_str_as_err(args[1]);
	}
	return beryl_retain(args[0]);
}

KHS_BOOL_OP(and_callback, "and?", &&)
KHS_BOOL_OP(or_callback, "or?", ||)

KHS_CMP_OP(less_callback, 
  int cmp_res = beryl_val_cmp(prev, next);
	if (cmp_res == 0 || cmp_res == -1) {
		return BERYL_FALSE;
  }
	if (cmp_res == 2) {
		beryl_blame_arg(prev);
		beryl_blame_arg(next);
		return BERYL_ERR("Uncomparable values");
	}
)

KHS_CMP_OP(greater_callback,
	int cmp_res = beryl_val_cmp(prev, next);
	if (cmp_res == 0 || cmp_res == 1) {
		return BERYL_FALSE;
  }
	if (cmp_res == 2) {
		beryl_blame_arg(prev);
		beryl_blame_arg(next);
		return BERYL_ERR("Uncomparable values");
	}
)

KHS_CMP_OP(less_eq_callback,
	int cmp_res = beryl_val_cmp(prev, next);
	if (cmp_res == -1) {
		return BERYL_FALSE;
  }
	if (cmp_res == 2) {
		beryl_blame_arg(prev);
		beryl_blame_arg(next);
		return BERYL_ERR("Uncomparable values");
	}
)

KHS_CMP_OP(greater_eq_callback,
	int cmp_res = beryl_val_cmp(prev, next);
	if(cmp_res == 1) {
		return BERYL_FALSE;
  }
	if(cmp_res == 2) {
		beryl_blame_arg(prev);
		beryl_blame_arg(next);
		return BERYL_ERR("Uncomparable values");
	}
)

static i_val foreach_in_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	switch (BERYL_TYPEOF(args[0])) {
		case TYPE_TABLE: {
			i_val_pair *iter = NULL;
			i_val res = BERYL_NULL;
			while ((iter = beryl_iter_table(args[0], iter))) {
				beryl_release(res);
				i_val iter_args[] = { iter->key, iter->val };
				res = beryl_call(args[1], iter_args, 2, true);
				if (BERYL_TYPEOF(res) == TYPE_ERR)
					return res;
			}
			return res; }
		case TYPE_ARRAY: {
			i_val res = BERYL_NULL;
			khs_size len = BERYL_LENOF(args[0]);
			const i_val *items = beryl_get_raw_array(args[0]);
			for (khs_size i = 0; i < len; i++) {
				beryl_release(res);
				i_val iter_args[] = { BERYL_NUMBER(i), items[i] };
				res = beryl_call(args[1], iter_args, 2, true);
				if (BERYL_TYPEOF(res) == TYPE_ERR) {
					return res;
        }
			}
			return res; }
		default:
			beryl_blame_arg(args[0]);
			return BERYL_ERR("Expected array or table as argument for 'foreach-in'");
	}
}

static void union_tables(i_val *into_table, i_val from_table) {
	assert(beryl_get_refcount(*into_table) == 1);
	khs_size remaining_cap = into_table->val.table->cap - BERYL_LENOF(*into_table);
	assert(BERYL_LENOF(from_table) <= remaining_cap); (void) remaining_cap;
	i_val_pair *iter = NULL;
	while ((iter = beryl_iter_table(from_table, iter))) {
		beryl_table_insert(into_table, iter->key, iter->val, false);
	}
}

static i_val insert_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) != TYPE_TABLE) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected table as first argument for 'insert'");
	}
	i_val table = args[0];
	khs_refc refs = beryl_get_refcount(table);
	bool retain_table = false;
  if (refs != 1) {
		i_val new_table = beryl_new_table(BERYL_LENOF(table) + 1, true);
		if (BERYL_TYPEOF(new_table) == TYPE_NULL) {
			return BERYL_ERR("Out of memory");
    }
		union_tables(&new_table, args[0]);
		table = new_table;
	} 
  else {
    if (beryl_table_should_grow(table, 1)) {
			i_val new_table = beryl_new_table(BERYL_LENOF(table) + 1, true);
			if (BERYL_TYPEOF(new_table) == TYPE_NULL)
				return BERYL_ERR("Out of memory");
			union_tables(&new_table, table);
			table = new_table;
		} 
    else {
			retain_table = true;
    }
	}
	int err = beryl_table_insert(&table, args[1], args[2], false);
	if (retain_table) {
		beryl_retain(table);
  }
	if (err == 0) {
		return table;
  }
	beryl_release(table);
	switch (err) {
		case 3:
			beryl_blame_arg(args[1]);
			return BERYL_ERR("Invalid table key");
		case 2:
			beryl_blame_arg(args[1]);
			return BERYL_ERR("Duplicate key");
		case 1:
			assert(false);
			return BERYL_NULL;
		default:
			assert(false);
			return BERYL_NULL;
	}
}

static i_val union_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) != TYPE_TABLE) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected table as first argument for 'union:'");
	}
	if (BERYL_TYPEOF(args[1]) != TYPE_TABLE) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Expected table as second argument for 'union:'");
	}
	khs_size a_len = BERYL_LENOF(args[0]);
	khs_size b_len = BERYL_LENOF(args[1]);
	if (KHS_SIZE_MAX - a_len < b_len) {
		return BERYL_ERR("Out of memory");
  }
	khs_size total_len = a_len + b_len;
	i_val new_table = beryl_new_table(total_len, true);
	if (BERYL_TYPEOF(new_table) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	union_tables(&new_table, args[0]);
	union_tables(&new_table, args[1]);
	return new_table;
}

static i_val concat_callback(const i_val *args, khs_size n_args) {
	i_val err;
	bool only_strings = true;
	for (khs_size i = 0; i < n_args; i++) {
		if (BERYL_TYPEOF(args[i]) != TYPE_STR) {
			only_strings = false;
		}
	}
	const i_val *strings = args;
	i_val *str_array = NULL;
	if (!only_strings) {
		str_array = beryl_talloc(n_args * sizeof(i_val));
		for (khs_size i = 0; i < n_args; i++) {
			str_array[i] = i_val_as_string(args[i]);
    }
		strings = str_array;
	}
	khs_size total_len = 0;
	for (khs_size i = 0; i < n_args; i++) {
		if (BERYL_TYPEOF(strings[i]) == TYPE_ERR) {
			err = beryl_retain(strings[i]);
			goto ERR;
		}
		total_len += BERYL_LENOF(strings[i]);
	}
	i_val res_str = beryl_new_string(total_len, NULL);
	if (BERYL_TYPEOF(res_str) == TYPE_NULL) {
		err = BERYL_ERR("Out of memory");
		goto ERR;
	}
	char *raw_str = (char *) beryl_get_raw_str(&res_str);
	char *s = raw_str;
	for (khs_size i = 0; i < n_args; i++) {
		khs_size len = BERYL_LENOF(strings[i]);
		if (len > 0) {
			assert (s < raw_str + total_len);
		}
		mcpy(s, beryl_get_raw_str(&strings[i]), len);
		s += len;
	}
	if (str_array != NULL) {
		for(size_t i = 0; i < n_args; i++)
			beryl_release(str_array[i]);
		beryl_tfree(str_array);
	}
	return res_str;
ERR:
	if (str_array != NULL) {
		for (size_t i = 0; i < n_args; i++) {
			beryl_release(str_array[i]);
    }
		beryl_tfree(str_array);
	}
	return err;
}

static i_val in_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	i_val index_res = beryl_call(args[1], &args[0], 1, false);
	if (BERYL_TYPEOF(index_res) == TYPE_ERR) {
		return index_res;
  }
	i_val res = BERYL_BOOL(BERYL_TYPEOF(index_res) != TYPE_NULL);
	beryl_release(index_res);
	return res;
}

static i_val sizeof_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	switch (BERYL_TYPEOF(args[0])) {
		case TYPE_STR:
		case TYPE_TABLE:
		case TYPE_ARRAY:
			return BERYL_NUMBER(BERYL_LENOF(args[0]));
		default:
			return BERYL_NUMBER(1);
	}
}

static i_val replace_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	switch (BERYL_TYPEOF(args[0])) {
		case TYPE_TABLE: {
			i_val index_res = beryl_pcall(args[0], &args[1], 1, true, false); 
			if (BERYL_TYPEOF(index_res) == TYPE_NULL) {
				beryl_blame_arg(args[1]);
				beryl_release(index_res);
				return BERYL_ERR("Key not in struct");
			}
			beryl_release(index_res);
      if (beryl_get_refcount(args[0]) == 1) {
				i_val res = args[0];
				beryl_table_insert(&res, args[1], args[2], true);
				return beryl_retain(res);
			} 
      else {
				i_val new_table = beryl_new_table(BERYL_LENOF(args[0]) + 1, true);
				if (BERYL_TYPEOF(new_table) == TYPE_NULL)	{
					return BERYL_ERR("Out of memory");
        }
				beryl_table_insert(&new_table, args[1], args[2], false);
				union_tables(&new_table, args[0]);
				return new_table;
			} }
		default:
			beryl_blame_arg(args[0]);
			return BERYL_ERR("Can only use 'replace' on structs");
	}
}

void beryl_core_lib_clear_evals() {
	eval_item *next = NULL;
	for (eval_item *p = eval_list; p != NULL; p = next) {
		next = p->prev;
		beryl_release(p->item);
		beryl_free(p);
	}
}

static i_val try_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	int mode;
  if (beryl_val_cmp(args[1], catch_tag) == 0) {
		mode = 0;
	} 
  else if (beryl_val_cmp(args[1], else_tag) == 0) {
		mode = 1;
	} 
  else if (beryl_val_cmp(args[1], print_trace_tag) == 0) {
		mode = 2;
	} 
  else {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Expected 'catch' or 'else'");
	}
	i_val res = beryl_pcall(args[0], NULL, 0, true, mode == 2);
	if (BERYL_TYPEOF(res) != TYPE_ERR) {
		return res;
  }
  if (mode == 1) {
		beryl_release(res);
		return beryl_retain(args[2]);
	} 
  else {
		i_val err_msg = beryl_err_as_str(res);
		beryl_release(res);
		i_val res = beryl_call(beryl_retain(args[2]), &err_msg, 1, false);
		return res;
	}
}

static i_val eval_callback(const i_val *args, khs_size n_args) {
	if (n_args != 1 && n_args != 3) {
		return BERYL_ERR("`eval` takes either 1 or 3 arguments");
  }
	if (BERYL_TYPEOF(args[0]) != TYPE_STR) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected string as argument for 'eval'");
	}
	enum beryl_err_action err_action = BERYL_PROP_ERR;
	if (n_args == 3) {
    if (beryl_val_cmp(args[1], catch_tag) == 0) {
			err_action = BERYL_CATCH_ERR;
		} 
    else if (beryl_val_cmp(args[1], print_trace_tag) == 0) {
			err_action = BERYL_PRINT_ERR;
		} 
    else {
			beryl_blame_arg(args[1]);
			return BERYL_ERR("Expected 'catch'");
		}
	}
	eval_item *new_entry = beryl_alloc(sizeof(eval_item));
	if (new_entry == NULL) {
		return BERYL_ERR("Out of memory");
  }
	new_entry->prev = eval_list;
	new_entry->item = beryl_retain(args[0]);
	eval_list = new_entry;
	i_val res = beryl_eval(beryl_get_raw_str(&eval_list->item), BERYL_LENOF(eval_list->item), err_action);
	if (err_action != BERYL_PROP_ERR && BERYL_TYPEOF(res) == TYPE_ERR) {
		i_val handler_res = beryl_call(beryl_retain(args[2]), &res, 1, false);
		return handler_res;
	}
	return res;
}

static i_val filter_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) != TYPE_ARRAY) {
		beryl_blame_arg(args[0]);
		beryl_release_values(args, n_args);
		return BERYL_ERR("First argument of 'filter' must be array");
	}
	i_val res;
	const i_val *from_array = beryl_get_raw_array(args[0]);
	if (beryl_get_refcount(args[0]) == 1) {
		res = beryl_retain(args[0]);
  }
	else {
		khs_size len = BERYL_LENOF(args[0]);
		res = beryl_new_array(len, NULL, len, false);
		if (BERYL_TYPEOF(res) == TYPE_NULL) {
			beryl_release_values(args, n_args);
			return BERYL_ERR("Out of memory");
		}
		i_val *a = (i_val *)beryl_get_raw_array(res);
		for (khs_size i = 0; i < len; i++) {
			a[i] = BERYL_NULL;
    }
	}
	i_val *to_array = (i_val *)beryl_get_raw_array(res);
	khs_size res_n = 0;
	for (khs_size i = 0; i < BERYL_LENOF(args[0]); i++) {
		i_val filter_res = beryl_call(args[1], &from_array[i], 1, true);
    if (BERYL_TYPEOF(filter_res) == TYPE_ERR) {
			beryl_release(res);
			beryl_release_values(args, n_args);
			return filter_res;
		} 
    else if (BERYL_TYPEOF(filter_res) != TYPE_BOOL) {
			beryl_release(res);
			beryl_blame_arg(filter_res);
			beryl_release(filter_res);
			beryl_release_values(args, n_args);
			return BERYL_ERR("Expected filter function to return boolean");
		}
		if (beryl_as_bool(filter_res)) {
			i_val tmp = beryl_retain(from_array[i]);
			beryl_release(*to_array);
			*(to_array++) = tmp; 
			res_n++;
		}
	}
	beryl_release_values(args, n_args);
	res.len = res_n;
	return res;
}

static i_val push_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) != TYPE_ARRAY) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("First argument of push must be array");
	}
	khs_size len = BERYL_LENOF(args[0]);
	i_val array = BERYL_NULL;
  if (beryl_get_refcount(args[0]) == 1) {
		assert(args[0].managed);
		if (beryl_get_array_capacity(args[0]) > len) {
			array =	beryl_retain(args[0]);
    }
		else {
			array = beryl_new_array(len, beryl_get_raw_array(args[0]), len + 1, true);
    }
	} 
  else {
		array = beryl_new_array(len, beryl_get_raw_array(args[0]), len + 1, true);
	}
	if (BERYL_TYPEOF(array) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	assert(array.managed && BERYL_TYPEOF(array) == TYPE_ARRAY);
	assert(beryl_get_array_capacity(array) > BERYL_LENOF(array));
	i_val *items = (i_val*) beryl_get_raw_array(array);
	items[array.len++] = beryl_retain(args[1]);
	return array;
}

static i_val mod_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (!beryl_is_integer(args[0])) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Can only take modulus of integer numbers");
	}
	if (!beryl_is_integer(args[1])) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Can only take modulus of integer numbers");
	}
	long long t = beryl_as_num(args[0]);
	long long n = beryl_as_num(args[1]);
	long long res = ((t % n) + n) % n;
	return BERYL_NUMBER(res);
}

static i_val arrayof_capture_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(arrayof_array) == TYPE_NULL) {
		return BERYL_NULL;
  }
	if (n_args == 0) {
		return BERYL_NULL;
  }
  if (n_args == 1) {
		bool ok = beryl_array_push(&arrayof_array, args[0]);
		if (!ok) {
			return BERYL_ERR("Out of memory");
    }
	} 
  else {
		i_val new_array = beryl_new_array(n_args, args, n_args, false);
		if (BERYL_TYPEOF(new_array) == TYPE_NULL) {
			return BERYL_ERR("Out of memory");
    }
		bool ok = beryl_array_push(&arrayof_array, new_array);
		beryl_release(new_array);
		if (!ok) {
			return BERYL_ERR("Out of memory");
    }
	}
	return BERYL_NULL;
}

static i_val arrayof_callback(const i_val *args, khs_size n_args) {
  static beryl_external_fn arrayof_capture_fn = KHS_FN(-1, "arrayof-capture", arrayof_capture_callback);
	i_val *pass_args = beryl_talloc(sizeof(i_val) * n_args);
	if (pass_args == NULL) {
		return BERYL_ERR("Out of memory");
  }
	for (khs_size i = 1; i < n_args; i++) {
		pass_args[i-1] = args[i];
  }
	pass_args[n_args-1] = BERYL_EXT_FN(&arrayof_capture_fn);
	i_val prev_array = arrayof_array;
	arrayof_array = beryl_new_array(0, NULL, 4, false);
	if (BERYL_TYPEOF(arrayof_array) == TYPE_NULL) {
		arrayof_array = prev_array;
		beryl_tfree(pass_args);
		return BERYL_ERR("Out of memory");
	}
	i_val res = beryl_call(args[0], pass_args, n_args, true);
	beryl_tfree(pass_args);
	if (BERYL_TYPEOF(res) == TYPE_ERR) {
		beryl_release(arrayof_array);
		arrayof_array = prev_array;
		return res;
	}
	beryl_release(res);
	res = arrayof_array;
	arrayof_array = prev_array;
	return res;
}

static i_val construct_array_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (!beryl_is_integer(args[0])) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected integer number as first argument of 'construct-array'");
	}
	khs_float f = beryl_as_num(args[0]);
	if (f > KHS_SIZE_MAX) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Array would be too large");
	}
	khs_size len = f;
	i_val array = beryl_new_array(0, NULL, len, false);
	if (BERYL_TYPEOF(array) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	i_val *a = (i_val *) beryl_get_raw_array(array);
	for (khs_size i = 0; i < len; i++) {
		i_val arg = BERYL_NUMBER(i);
		i_val item = beryl_call(args[1], &arg, 1, true);
		beryl_release(arg);
		if (BERYL_TYPEOF(item) == TYPE_ERR) {
			beryl_release(array);
			return item;
		}
		a[i] = item;
	}
	array.len = len;
	return array;
}

static i_val loop_callback(const i_val *args, khs_size n_args) {
	(void) n_args;
	while (true) {
		i_val res = beryl_call(args[0], NULL, 0, true);
		if (BERYL_TYPEOF(res) == TYPE_ERR)
			return res;
		if (BERYL_TYPEOF(res) != TYPE_BOOL) {
			beryl_blame_arg(res);
			beryl_release(res);
			return BERYL_ERR("Expected loop body to return boolean");
		}
		if (!beryl_as_bool(res)) {
			break;
    }
	}
	return BERYL_NULL;
}

static bool match_str(const char *str, const char *str_end, const char *substr, size_t substr_len) {
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

static const char *find_str(const char *str, const char *str_end, const char *substr, size_t substr_len) {
	for (const char *c = str; c < str_end; c++) {
		if(match_str(c, str_end, substr, substr_len)) {
			return c;
    }
	}
	return NULL;
}

static const char *find_str_right(const char *str, const char *str_end, const char *substr, size_t substr_len) {
	for (const char *c = str_end - 1; c >= str; c--) {
		if (match_str(c, str_end, substr, substr_len)) {
			return c;
    }
	}
	return NULL;
}

static i_val find_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE2(TYPE_STR, "string", TYPE_STR, "string");
	khs_size str_len = BERYL_LENOF(args[0]);
	const char *str = beryl_get_raw_str(&args[0]);
	const char *str_end = str + str_len;
	const char *at = find_str(str, str_end, beryl_get_raw_str(&args[1]), BERYL_LENOF(args[1]));
	if (at == NULL) {
		return BERYL_NULL;
  }
	return BERYL_NUMBER(at - str);
}

static i_val beginswith_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE2(TYPE_STR, "string", TYPE_STR, "string");
	i_val res = find_callback(args, n_args);
	return BERYL_BOOL(beryl_val_cmp(res, BERYL_NUMBER(0)) == 0);
}

static i_val find_right_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE2(TYPE_STR, "string", TYPE_STR, "string");
	khs_size str_len = BERYL_LENOF(args[0]);
	const char *str = beryl_get_raw_str(&args[0]);
	const char *str_end = str + str_len;
	const char *at = find_str_right(str, str_end, beryl_get_raw_str(&args[1]), BERYL_LENOF(args[1]));
	if (at == NULL) {
		return BERYL_NULL;
  }
	return BERYL_NUMBER(at - str);
}

static i_val endswith_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE2(TYPE_STR, "string", TYPE_STR, "string");
	khs_size str_len = BERYL_LENOF(args[0]);
	const char *str = beryl_get_raw_str(&args[0]);
	const char *str_end = str + str_len;
	khs_size substr_len = BERYL_LENOF(args[1]);
	const char *at = find_str_right(str, str_end, beryl_get_raw_str(&args[1]), substr_len);
	if (at == NULL) {
		return BERYL_FALSE;
  }
	return BERYL_BOOL(str_end - substr_len == at);
}

static i_val substring_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE3(TYPE_STR, "string", TYPE_NUMBER, "number", TYPE_NUMBER, "number");
	if (!beryl_is_integer(args[1])) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Expected starting index as integer, got '%0'");
	}
	if (!beryl_is_integer(args[2])) {
		beryl_blame_arg(args[2]);
		return BERYL_ERR("Expected ending index as integer, got '%0'");
	}
	khs_float fi_f = beryl_as_num(args[1]);
	khs_float ti_f = beryl_as_num(args[2]);
	if (fi_f > ti_f) {
		beryl_blame_arg(args[1]);
		beryl_blame_arg(args[2]);
		return BERYL_ERR("Start index (%0) for substring is larger than end index (%1)");
	}
	if (fi_f < 0) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Substring start index (%0) is out of bounds");
	}
	if (ti_f > KHS_SIZE_MAX) {
		beryl_blame_arg(args[2]);
		return BERYL_ERR("Substring end index (%0) is out of bounds");
	}
	khs_size from = fi_f;
	khs_size to = ti_f;
	khs_size substr_len = to - from;
	if (to > BERYL_LENOF(args[0])) {
		beryl_blame_arg(args[2]);
		return BERYL_ERR("Substring end index (%0) is out of bounds");
	}
	const char *from_str = beryl_get_raw_str(&args[0]);
	assert(from_str + from + substr_len <= from_str + BERYL_LENOF(args[0]));
	i_val substr = beryl_new_string(substr_len, from_str + from);
	if (BERYL_TYPEOF(substr) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	return substr;
}

static i_val default_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) == TYPE_NULL) {
		return beryl_retain(args[1]);
  }
	else {
		return beryl_retain(args[0]);
  }
}

static bool str_buff_init(str_buff *buff) {
	buff->str = beryl_alloc(STR_BUFF_START_SIZE);
	buff->top = buff->str;
	buff->end = buff->str + STR_BUFF_START_SIZE;
	return buff->str != NULL;
}

static void str_buff_free(str_buff *buff) {
	beryl_free(buff->str);
	buff->str = NULL;
	buff->top = NULL;
	buff->end = NULL;
}

static bool str_buff_pushs(str_buff *buff, const char *str, size_t len) {
	char *p = buff->top;
	buff->top += len;
	if (buff->top > buff->end) {
		size_t cap = buff->end - buff->str;
		size_t blen = p - buff->str;
		cap = (cap * 3) / 2 + len;
		char *new_alloc = beryl_realloc(buff->str, cap);
		if (new_alloc == NULL) {
			buff->top -= len;
			return false;
		}
		buff->str = new_alloc;
		buff->top = buff->str + blen + len;
		buff->end = buff->str + cap;
		p = buff->str + blen;
	}
	mcpy(p, str, len);
	return true;
}

static i_val str_replace_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE3(TYPE_STR, "string", TYPE_STR, "string", TYPE_STR, "string");
	const char *str = beryl_get_raw_str(&args[0]);
	khs_size str_len = BERYL_LENOF(args[0]);
	const char *str_end = str + str_len;
	const char *replace = beryl_get_raw_str(&args[1]);
	khs_size replace_len = BERYL_LENOF(args[1]);
	const char *replace_with = beryl_get_raw_str(&args[2]);
	khs_size replace_with_len = BERYL_LENOF(args[2]);
	str_buff buff;
	bool ok = str_buff_init(&buff);
	if (!ok) {
		goto MEM_ERR;
  }
	const char *start = NULL;
	for (const char *c = str; c < str_end; c++) {
    if (match_str(c, str_end, replace, replace_len)) {
			if (start != NULL) {
				bool ok = str_buff_pushs(&buff, start, c - start);
				if (!ok) {
					goto MEM_ERR;	
        }
			}
			bool ok = str_buff_pushs(&buff, replace_with, replace_with_len);
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
		bool ok = str_buff_pushs(&buff, start, str_end - start);
		if (!ok) {
			goto MEM_ERR;
    }
	}
	size_t res_len = buff.top - buff.str;
	if (res_len > KHS_SIZE_MAX) {
		str_buff_free(&buff);
		return BERYL_ERR("Resulting string would be too large");
	}
	i_val res = beryl_new_string(res_len, buff.str);
	if (BERYL_TYPEOF(res) == TYPE_NULL) {
		goto MEM_ERR;
  }
	str_buff_free(&buff);
	return res;
MEM_ERR:
	str_buff_free(&buff);
	return BERYL_ERR("Out of memory");
}

static bool c_is_digit(char c) {
	return c >= '0' && c <= '9';
}

static int char_as_digit(char c) {
	assert(c_is_digit(c));
	return c - '0';
}

static i_val parse_number_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE1(TYPE_STR, "string");
	const char *str = beryl_get_raw_str(&args[0]);
	khs_size len = BERYL_LENOF(args[0]);
	const char *str_end = str + len;
	const char *c = str;
	for (; c != str_end; c++) {
		if (c_is_digit(*c) || *c == '-') {
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
		if (!c_is_digit(*c)) {
			break;
    }
		res *= 10;
		res += char_as_digit(*c);
	}
	if (c != str_end && *c == '.') {
		c++;
		khs_float pow = 1;
		for (; c != str_end; c++) {
			if (!c_is_digit(*c)) {
				break;
      }
			pow *= 0.1;
			res += pow * char_as_digit(*c);
		}
	}
	return BERYL_NUMBER(negative ? -res : res);
}

size_t iilog10(size_t i) {
	size_t n = 0;
	while (i /= 10) {
		n++;
  }
	return n;
}

size_t iflog10(khs_float f, khs_float *opt_out) {
	size_t n = 0;
	while ((f /= 10.0) > 1) {
		n++;
  }
	if (opt_out != NULL) {
		*opt_out = f * 10;
  }
	return n;
}

static char int_as_digit(int i) {
	assert(i >= 0 && i <= 9);
	return '0' + i;
}

static void init_stringf(stringf *f, char *str, size_t len) {
	f->str = str;
	f->end = str + len;
	f->at = str;
}

static void write_char(stringf *f, char c) {
	assert(f->at < f->end);
	*(f->at++) = c;
}

static void write_char_at(stringf *f, size_t offset, char c) {
	char *s = f->at + offset;
	assert(s >= f->str && s < f->end);
	*s = c;
}

static void	move_cursor(stringf *f, size_t offset) {
	f->at += offset;
	assert(f->at <= f->end);
}


static i_val num_to_str(khs_float num) {
	bool negative = num < 0;
	if(negative) {
		num = -num;
  }
	if (num / 10 == num) {
		if (num == 0) {
			return BERYL_CONST_STR("0");
    }
		if (negative) {
			return BERYL_CONST_STR("-infinity");
    }
		else {
			return BERYL_CONST_STR("infinity");
    }
	}
	if (num > KHS_LARGE_UINT_TYPE_MAX) {
		khs_float	significand;
		size_t exp = iflog10(num, &significand);
		size_t n_sig_digits = 4;
		size_t n_exp_digits = iilog10(exp) + 1;
		size_t n_total = (negative ? 1 : 0) + (n_sig_digits + 2) + 2 + n_exp_digits;
		i_val res = beryl_new_string(n_total, NULL);
		if (BERYL_TYPEOF(res) == TYPE_NULL) {
			return BERYL_NULL;
    }
		stringf s;
		init_stringf(&s, (char *) beryl_get_raw_str(&res), n_total);
		if (negative) {
			write_char(&s, '-');
		}
		write_char(&s, int_as_digit(significand));
		write_char(&s, '.');
		for (size_t i = 0; i < n_sig_digits; i++) {
			significand -= (int) significand;
			significand *= 10;
			write_char(&s, int_as_digit(significand));
		}
		write_char(&s, 'e');
		write_char(&s, '+');
		for (size_t i = n_exp_digits - 1; true; i--) {
			write_char_at(&s, i, int_as_digit(exp % 10));
			exp /= 10;
			if (i == 0) {
				break;
      }
		}
		return res;
	}
	khs_large_uint_type int_v = num;
	size_t n_idigits = iilog10(int_v) + 1;
	size_t n_ddigits = beryl_is_integer(BERYL_NUMBER(num)) ? 0 : 4;
	size_t n_total = (negative ? 1 : 0) + n_idigits + n_ddigits;
	if (n_ddigits != 0) {
		n_total += 1;
  }
	i_val res = beryl_new_string(n_total, NULL);
	if (BERYL_TYPEOF(res) == TYPE_NULL) {
		return BERYL_NULL;
  }
	stringf s;
	init_stringf(&s, (char *) beryl_get_raw_str(&res), n_total);
	if (negative) {
		write_char(&s, '-');
  }
	khs_large_uint_type i_num = int_v;
	for (size_t i = n_idigits - 1; true; i--) {
		char digit = int_as_digit(i_num % 10);
		i_num /= 10;
		write_char_at(&s, i, digit);
		if (i == 0) {
			break;
    }
	}
	move_cursor(&s, n_idigits);
	if (n_ddigits != 0) {
		write_char(&s, '.');
  }
	khs_float dec_part = num - int_v;
	for (size_t i = 0; i < n_ddigits; i++) {
		dec_part *= 10;
		char digit = int_as_digit(((khs_large_uint_type) dec_part) % 10);
		write_char_at(&s, i, digit);
	}
	return res;
}

i_val i_val_as_string(i_val val) {
	switch (BERYL_TYPEOF(val)) {
		case TYPE_NUMBER: {
			i_val res = num_to_str(beryl_as_num(val));
			if (BERYL_TYPEOF(res) == TYPE_NULL) {
				return BERYL_ERR("Out of memory");
      }
			return res; }
		case TYPE_FN:
		case TYPE_EXT_FN:
			return BERYL_CONST_STR("Function");
		case TYPE_ARRAY:
			return BERYL_CONST_STR("Array");
		case TYPE_TAG:
			return BERYL_CONST_STR("Tag");
		case TYPE_NULL:
			return BERYL_CONST_STR("Null");
		case TYPE_STR:
			return beryl_retain(val);
		case TYPE_TABLE:
			return BERYL_CONST_STR("Table");
		default:
			return BERYL_CONST_STR("Unkown");
	}
}

static i_val as_string_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	return i_val_as_string(args[0]);
}

static i_val round_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	EXPECT_TYPE1(TYPE_NUMBER, "number");
	khs_float num = beryl_as_num(args[0]);
	bool negative = num < 0;
	if (negative) {
		num = -num;
  }
	if (num > KHS_NUM_MAX_INT) {
		return beryl_retain(args[0]);
  }
	khs_large_uint_type i = (khs_large_uint_type) (num + 0.5);
	khs_float res = i;
	if (negative) {
		res = -res;
  }
	return BERYL_NUMBER(res);
}

static i_val is_int_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	return BERYL_BOOL(beryl_is_integer(args[0]));
}

static i_val pipe_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	i_val res = beryl_call(args[1], &args[0], 1, false);
	return res;
}

static i_val error_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[0]) != TYPE_STR) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected string error message as argument for 'error', got '%0'");
	}
	for (khs_size i = 1; i < n_args; i++) {
		beryl_blame_arg(args[i]);	
  }
	return beryl_str_as_err(args[0]);
}

static khs_size bt_parent(khs_size i) {
	assert(i != 0);
	return (i - 1) / 2;
}

static khs_size bt_left_child(khs_size i) {
	return i * 2 + 1;
}

static khs_size bt_right_child(khs_size i) {
	return i * 2 + 2;
}

static bool is_inside_heap(khs_size i, khs_size len) {
	return i < len;
}

static inline int checked_cmp(i_val a, i_val b) {
	int res = beryl_val_cmp(a, b);
	if(res == 2 && !sort_err) {
		sort_err = true;
		beryl_blame_arg(a);
		beryl_blame_arg(b);
	}
	return res;
}

static void make_heap(i_val *array, khs_size at, khs_size len) {
START:
	assert(at < len);
	i_val top = array[at];
	khs_size left_i = bt_left_child(at);
	khs_size right_i = bt_right_child(at);
	if (!is_inside_heap(left_i, len)) {
		assert(!is_inside_heap(right_i, len));
		return;
	}
	i_val left = array[left_i];
	if (!is_inside_heap(right_i, len)) {
    if (checked_cmp(left, top) == -1) {
			array[at] = left;
			array[left_i] = top;
			at = left_i;
			goto START;
		} 
    else
			return;
	}
	assert(is_inside_heap(right_i, len));
	i_val right = array[right_i];
	if (checked_cmp(left, top) == -1) {
    if (checked_cmp(left, right) == -1) {
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
	if (checked_cmp(right, top) == -1) {
    if (checked_cmp(right, left) == -1) {
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

static i_val sort_array(i_val *array, khs_size len) {
	for(khs_size i = bt_parent(len - 1); true; i--) {
		make_heap(array, i, len);
		if (i == 0) {
			break;
    }
	}
	for (khs_size heap_len = len; heap_len > 1; heap_len--) {
		i_val max = array[0];
		khs_size leaf_i = heap_len - 1;
		array[0] = array[leaf_i];
		array[leaf_i] = max;
		make_heap(array, 0, heap_len - 1);
	}
	if (sort_err) {
		sort_err = false;
		return BERYL_ERR("Cannot compare values '%0' and '%1'");
	}
	return BERYL_NULL;
}

static i_val sort_callback(const i_val *args, khs_size n_args) {
	(void)n_args;
	if (BERYL_TYPEOF(args[0]) != TYPE_ARRAY) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Can only sort arrays");
	}
	i_val array_to_sort;
	khs_size len = BERYL_LENOF(args[0]);
  if (beryl_get_refcount(args[0]) == 1) {
		array_to_sort = beryl_retain(args[0]);
	} 
  else {
		array_to_sort = beryl_new_array(len, beryl_get_raw_array(args[0]), len, false); //Creates a copy of the array
	}
	if (BERYL_TYPEOF(array_to_sort) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	i_val err = sort_array((i_val *) beryl_get_raw_array(array_to_sort), len);
	if (BERYL_TYPEOF(err) == TYPE_ERR) {
		beryl_release(array_to_sort);
		return err;
	}
	return array_to_sort;
}

static khs_large_uint_type random_from(khs_large_uint_type from) {
	for(size_t i = 0; i < TAPE_LEN; i++) {
		tape[i] *= tape[i] + (from+1);
	}
	size_t tape_head = 0;
	khs_large_uint_type res = from;
	for (khs_large_uint_type i = 1; i != 0; i = i << 1) {
		char bit = from & 1;
		from = from >> 1;
		if (bit) {
			tape_head += res + 1;
    }
		else {
			tape_head -= res + 1;
    }
		res *= res;
		res += tape[tape_head % TAPE_LEN];
	}
	return res;
}

static i_val random_callback(const i_val *args, khs_size n_args) {
	(void)n_args, (void)args;
	static khs_large_uint_type seed = (khs_large_uint_type) &random_callback;
	seed = random_from(seed);
	return BERYL_NUMBER((khs_float) (seed) / (khs_float) KHS_LARGE_UINT_TYPE_MAX);
}

static i_val slice_array(i_val array, khs_size from, khs_size to) {
	assert(BERYL_TYPEOF(array) == TYPE_ARRAY);
	assert(from <= to);	
	assert(to <= BERYL_LENOF(array));
	const i_val *items = beryl_get_raw_array(array);
	if (from == 0 && beryl_get_refcount(array) == 1) {
		for (khs_size i = BERYL_LENOF(array) - 1; i >= to; i--) {
			beryl_release(items[i]);
			if (i == 0) {
				break;
      }
		}
		array.len = to;
		return beryl_retain(array);
	}
	khs_size len = to - from;
	assert(from + len <= BERYL_LENOF(array));
	i_val res = beryl_new_array(len, items + from, len, false);
	if (BERYL_TYPEOF(res) == TYPE_ERR) {
		return BERYL_ERR("Out of memory");
  }
	return res;
}

static i_val slice_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE3(TYPE_ARRAY, "array", TYPE_NUMBER, "number", TYPE_NUMBER, "number"); 
  (void)n_args;
	if (!beryl_is_integer(args[1])) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Can only use integers as array indicies");
	}
	if (!beryl_is_integer(args[2])) {
		beryl_blame_arg(args[2]);
		return BERYL_ERR("Can only use integers as array indicies");
	}
	khs_float from_n = beryl_as_num(args[1]);
	khs_float to_n = beryl_as_num(args[2]);
	if (to_n > KHS_SIZE_MAX) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Index out of range");
	}
	if (from_n > to_n) {
		beryl_blame_arg(args[1]);
		beryl_blame_arg(args[2]);
		return BERYL_ERR("'From' index cannot be larger than 'to' index");
	}
	khs_size from = from_n;
	khs_size to = to_n;
	if (from_n < 0) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("'From' index out of range");
	}
	if (to > BERYL_LENOF(args[0])) {
		beryl_blame_arg(args[2]);
		return BERYL_ERR("'To' index out of range");
	}
	return slice_array(args[0], from, to);
}

static i_val pop_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE1(TYPE_ARRAY, "array"); 
  (void)n_args;
	if (BERYL_LENOF(args[0]) == 0) {
		return BERYL_ERR("Cannot pop empty array");
	}
	return slice_array(args[0], 0, BERYL_LENOF(args[0]) - 1);
}

static i_val peek_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE1(TYPE_ARRAY, "array"); (void) n_args;
	khs_size len = BERYL_LENOF(args[0]);
	if (len == 0) {
		return BERYL_ERR("Cannot peek empty array");
  }
	return beryl_retain(beryl_get_raw_array(args[0])[len - 1]);
}

static i_val apply_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[1]) != TYPE_ARRAY) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Can only apply arrays to functions");
	}
	return beryl_call(args[0], beryl_get_raw_array(args[1]), BERYL_LENOF(args[1]), true);
}

static i_val join_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE2(TYPE_ARRAY, "array", TYPE_ARRAY, "array");
	khs_size alen = BERYL_LENOF(args[0]);
	khs_size blen = BERYL_LENOF(args[1]);
	if (blen > KHS_SIZE_MAX - alen) {
		return BERYL_ERR("Out of memory");
  }
	khs_size res_len = alen + blen;
	i_val res_array = beryl_new_array(res_len, NULL, res_len, false);
	if (BERYL_TYPEOF(res_array) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	const i_val *a_a = beryl_get_raw_array(args[0]);
	const i_val *b_a = beryl_get_raw_array(args[1]);
	i_val *res_a = (i_val *) beryl_get_raw_array(res_array);
	while (alen--) {
		*(res_a++) = beryl_retain(*(a_a++));
	}
	while (blen--) {
		*(res_a++) = beryl_retain(*(b_a++));
	}
	return res_array;
}

static i_val map_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[0]) != TYPE_ARRAY) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Can only 'map' arrays");
	}
	khs_size len = BERYL_LENOF(args[0]);
	i_val map_array;
  if (beryl_get_refcount(args[0]) == 1) {
		map_array = beryl_retain(args[0]);
	} 
  else {
		map_array = beryl_new_array(len, NULL, len, 0);
		if (BERYL_TYPEOF(map_array) == TYPE_NULL) {
			return BERYL_ERR("Out of memory");
    }
		i_val *a = (i_val *) beryl_get_raw_array(map_array);
		for (khs_size i = 0; i < len; i++) {
			a[i] = BERYL_NULL;
		}
	}
	assert(BERYL_TYPEOF(map_array) == TYPE_ARRAY);
	assert(BERYL_LENOF(map_array) >= len);
	i_val *dst = (i_val *) beryl_get_raw_array(map_array);
	const i_val *src = beryl_get_raw_array(args[0]);
	for (khs_size i = 0; i < len; i++) {
		i_val arg = beryl_retain(src[i]);
		beryl_release(dst[i]);
		i_val res = beryl_call(beryl_retain(args[1]), &arg, 1, false);
		if (BERYL_TYPEOF(res) == TYPE_ERR) {
			beryl_release(map_array);
			return res;
		}
		dst[i] = res;
	}
	return map_array;
}

static i_val forevery_callback(const i_val *args, khs_size n_args) { // DOESN'T USE AUTORELEASE
	i_val res = BERYL_NULL;
	i_val fn = args[n_args - 1];
	assert(n_args > 0);
	for (khs_size i = 0; i < n_args - 1; i++) {
		beryl_release(res);
		res = beryl_call(beryl_retain(fn), &args[i], 1, false);
		if (BERYL_TYPEOF(res) == TYPE_ERR) {
			for (khs_size j = i + 1; j < n_args - 1; j++) {
				beryl_release(args[i]);
      }
			beryl_release(fn);
			return res;
		}
	}
	beryl_release(fn);
	return res;
}

static i_val strip_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE1(TYPE_STR, "string");
	const char *str = beryl_get_raw_str(&args[0]);
	khs_size len = BERYL_LENOF(args[0]);
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
	i_val new_str = beryl_new_string((end - start) + 1, start);
	if(BERYL_TYPEOF(new_str) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	return new_str;
}

static i_val split_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE2(TYPE_STR, "string", TYPE_STR, "string");
	const char *str = beryl_get_raw_str(&args[0]);
	khs_size len = BERYL_LENOF(args[0]);
	const char *str_end = str + len;
	const char *split_at = beryl_get_raw_str(&args[1]);
	khs_size split_at_len = BERYL_LENOF(args[1]);
	if (split_at_len == 0) {
		return BERYL_ERR("String splitter cannot be an empty string");
	}
	khs_size tmp_buff_cap = 4, tmp_buff_len = 0;
	i_val *tmp_buff = beryl_alloc(tmp_buff_cap * sizeof(i_val));
	if (tmp_buff == NULL) {
		return BERYL_ERR("Out of memory");
  }
	const char *prev = NULL;
	for (const char *c = str; c < str_end;) {
    if (match_str(c, str_end, split_at, split_at_len)) {
			i_val new_str;
			if (prev == NULL) {
				new_str = BERYL_CONST_STR("");
      }
			else {
				new_str = beryl_new_string(c - prev, prev);
      }
			if (BERYL_TYPEOF(new_str) == TYPE_NULL) {
				goto MEM_ERR;
      }
      if (tmp_buff_len == tmp_buff_cap) {
				tmp_buff_cap *= 2;
				if (tmp_buff_cap <= tmp_buff_len) {
					beryl_release(new_str);
					goto MEM_ERR;
				}
				i_val *tmp = beryl_realloc(tmp_buff, tmp_buff_cap * sizeof(i_val));
				if (tmp == NULL) {
					beryl_release(new_str);
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
	i_val new_str;
	if (prev == NULL) {
		new_str = BERYL_CONST_STR("");
  }
	else {
		new_str = beryl_new_string(str_end - prev, prev);
  }
	if (BERYL_TYPEOF(new_str) == TYPE_NULL) {
		goto MEM_ERR;
  }
	if (tmp_buff_len == tmp_buff_cap) {
		tmp_buff_cap += 1;
		if (tmp_buff_cap <= tmp_buff_len) {
			beryl_release(new_str);
			goto MEM_ERR;
		}
		i_val *tmp = beryl_realloc(tmp_buff, tmp_buff_cap * sizeof(i_val));
		if (tmp == NULL) {
			beryl_release(new_str);
			goto MEM_ERR;
		}
		tmp_buff = tmp;
	}
	tmp_buff[tmp_buff_len++] = new_str;
	i_val res_array = beryl_new_array(tmp_buff_len, tmp_buff, tmp_buff_len, false);
	for (khs_size i = 0; i < tmp_buff_len; i++) {
		beryl_release(tmp_buff[i]);
  }
	beryl_free(tmp_buff);
	if (BERYL_TYPEOF(res_array) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	return res_array;
MEM_ERR:
	for (khs_size i = 0; i < tmp_buff_len; i++) {
		beryl_release(tmp_buff[i]);
  }
	beryl_free(tmp_buff);
	return BERYL_ERR("Out of memory");
}

static i_val identity_callback(const i_val *args, khs_size n_args) {
	return beryl_retain(args[0]);
}

static i_val typeof_callback(const i_val *args, khs_size n_args) {
	switch (BERYL_TYPEOF(args[0])) {
		case TYPE_NUMBER:
			return BERYL_CONST_STR("number");
		case TYPE_TABLE:
			return BERYL_CONST_STR("struct");
		case TYPE_ARRAY:
			return BERYL_CONST_STR("array");
		case TYPE_NULL:
			return BERYL_CONST_STR("null");
		case TYPE_STR:
			return BERYL_CONST_STR("string");
		case TYPE_FN:
		case TYPE_EXT_FN:
			return BERYL_CONST_STR("function");
		case TYPE_BOOL:
			return BERYL_CONST_STR("bool");
		case TYPE_TAG:
			return BERYL_CONST_STR("tag");
		case TYPE_OBJECT:
			return BERYL_CONST_STR("object");
		default:
			assert(false);
			return BERYL_CONST_STR("unkown");
	}
}

static i_val join_with_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE2(TYPE_ARRAY, "array", TYPE_STR, "string");
	khs_size items_total_len = 0;
	const i_val *items = beryl_get_raw_array(args[0]);
	khs_size n_items = BERYL_LENOF(args[0]);
	if (n_items == 0) {
		return BERYL_CONST_STR("");
  }
	for (khs_size i = 0; i < n_items; i++) {
		if (BERYL_TYPEOF(items[i]) != TYPE_STR) {
			beryl_blame_arg(items[i]);
			return BERYL_ERR("Array passed to 'join-with' must contain strings only");
		}
		khs_size len = BERYL_LENOF(items[i]);
		if (len > KHS_SIZE_MAX - items_total_len) {
			return BERYL_ERR("Resulting string would be too large");
    }
		items_total_len += len;
	}
	i_val join_str = args[1];
	assert(n_items >= 1);
	khs_size n_joiners = n_items - 1;
	if (BERYL_LENOF(join_str) != 0 && n_joiners > KHS_SIZE_MAX / BERYL_LENOF(join_str)) {
		return BERYL_ERR("Resulting string would be too large");
  }
	khs_size total_joiners_len = n_joiners * BERYL_LENOF(join_str);
	if (total_joiners_len > KHS_SIZE_MAX - items_total_len) {
		return BERYL_ERR("Resulting string would be too large");
  }
	khs_size total_len = items_total_len + total_joiners_len;
	i_val new_str = beryl_new_string(total_len, NULL);
	if (BERYL_TYPEOF(new_str) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	char *str = (char *) beryl_get_raw_str(&new_str);
	const char *str_end = str + total_len;
	for (khs_size i = 0; i < n_items; i++) {
		i_val item = items[i];
		khs_size len = BERYL_LENOF(item);
		mcpy(str, beryl_get_raw_str(&item), len);
		str += len;
		assert(str <= str_end);
		if (i != n_items - 1) {
			mcpy(str, beryl_get_raw_str(&join_str), BERYL_LENOF(join_str));
			str += BERYL_LENOF(join_str);
			assert(str <= str_end);
		}
	}
	return new_str;
}

static i_val repeat_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE2(TYPE_STR, "string", TYPE_NUMBER, "number");
	if (!beryl_is_integer(args[1])) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Can only repeat strings an integer number of times");
	}
	khs_float num = beryl_as_num(args[1]);
	if (num < 0 || num > KHS_SIZE_MAX) {
		beryl_blame_arg(args[1]);
		return BERYL_ERR("Invalid number of repetitions");
	}
	khs_size len = BERYL_LENOF(args[0]);
	khs_size times = num;
	if (len == 0 || times == 0) {
		return BERYL_CONST_STR("");
  }
	if (times > KHS_SIZE_MAX / len) {
		return BERYL_ERR("Resulting string would be too large");
  }
	khs_size total_len = times * len;
	i_val str_res = beryl_new_string(total_len, NULL);
	if(BERYL_TYPEOF(str_res) == TYPE_NULL) {
		return BERYL_ERR("Out of memory");
  }
	const char *src_str = beryl_get_raw_str(&args[0]);
	char *str = (char *)beryl_get_raw_str(&str_res);
	const char *str_end = str + total_len;
	for (khs_size i = 0; i < times; i++) {
		mcpy(str, src_str, len);
		str += len;
		assert(str <= str_end);
	}
	return str_res;
}

static i_val max_callback(const i_val *args, khs_size n_args) {
	i_val max = args[0];
	for (khs_size i = 1; i < n_args; i++) {
		int cmp = beryl_val_cmp(args[i], max);
		if (cmp == 2) {
			beryl_blame_arg(max); beryl_blame_arg(args[i]);
			return BERYL_ERR("Uncomparable values");
		}
		if (cmp == -1) {
			max = args[i];
    }
	}
	return beryl_retain(max);
}

static i_val find_in_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[0]) != TYPE_ARRAY) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected array as first argument for 'find-in'");
	}
	const i_val *a = beryl_get_raw_array(args[0]);
	khs_size len = BERYL_LENOF(args[0]);
	for (khs_size i = 0; i < len; i++) {
		if (beryl_val_cmp(a[i], args[1]) == 0) {
			return BERYL_NUMBER(i);
    }
	}
	return BERYL_NULL;
}

static i_val floor_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE1(TYPE_NUMBER, "number");
	if (beryl_is_integer(args[0])) {
		return beryl_retain(args[0]);
  }
	return BERYL_NUMBER((khs_large_uint_type) beryl_as_num(args[0]));
}

static i_val ceil_callback(const i_val *args, khs_size n_args) {
	EXPECT_TYPE1(TYPE_NUMBER, "number");
	if (beryl_is_integer(args[0])) {
		return beryl_retain(args[0]);
  }
	khs_float res = (khs_large_uint_type) beryl_as_num(args[0]);
	res += 1;
	return BERYL_NUMBER(res);
}

static i_val first_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[0]) != TYPE_ARRAY) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected array as first argument");
	}
	const i_val *a = beryl_get_raw_array(args[0]);
	for (khs_size i = 0; i < BERYL_LENOF(args[0]); i++) {
		i_val res = beryl_call(args[1], &a[i], 1, true);
		if (BERYL_TYPEOF(res) != TYPE_BOOL) {
			beryl_blame_arg(res);
			beryl_release(res);
			return BERYL_ERR("Expected boolean as result from callback function, got '%0'");
		}
		if (beryl_as_bool(res)) {
			return BERYL_NUMBER(i);
    }
	}
	return BERYL_NULL;
}

static i_val exists_callback(const i_val *args, khs_size n_args) {
	i_val res = first_callback(args, n_args);
	if (BERYL_TYPEOF(res) == TYPE_ERR) {
		return res;
  }
	return BERYL_BOOL(BERYL_TYPEOF(res) != TYPE_NULL);
}

static i_val all_callback(const i_val *args, khs_size n_args) {
	if (BERYL_TYPEOF(args[0]) != TYPE_ARRAY) {
		beryl_blame_arg(args[0]);
		return BERYL_ERR("Expected array as first argument");
	}
	const i_val *a = beryl_get_raw_array(args[0]);
	for (khs_size i = 0; i < BERYL_LENOF(args[0]); i++) {
		i_val res = beryl_call(args[1], &a[i], 1, true);
		if (BERYL_TYPEOF(res) == TYPE_ERR) {
			return res;
    }
		if (BERYL_TYPEOF(res) != TYPE_BOOL) {
			beryl_blame_arg(res);
			beryl_release(res);
			return BERYL_ERR("Expected boolean as result from callback function, got '%0'");
		}
		if (!beryl_as_bool(res)) {
			return BERYL_FALSE;
    }
	}
	return BERYL_TRUE;
}

static size_t istrlen(const char *str) {
	size_t l = 0;
	while (*str != '\0') {
		str++;
		l++;
	}
	return l;
}

bool load_core_lib() {
	static struct beryl_external_fn fns[] = {
	  KHS_FN(-3, "+", add_callback),
		KHS_FN(-2, "-", sub_callback),
		KHS_FN(-3, "*", mul_callback),
		KHS_FN(-3, "/", div_callback),
		KHS_FN(-3, "if", if_callback),
		KHS_FN(2, "==", eq_callback),
		KHS_FN(2, "=/=", not_eq_callback),
		KHS_FN(1, "not", not_callback),
		KHS_FN(-1, "array", array_callback),
		KHS_FN(2, "for-in", for_in_callback),
		KHS_FN(3, "for", for_callback),
		KHS_FN(2, "foreach-in", foreach_in_callback),
		KHS_FN(-1, "table", table_callback),
		KHS_FN(-1, "struct", table_callback),
		KHS_FN(0, "tag", tag_callback),
		KHS_MANUAL_RELEASE_FN(1, "invoke", invoke_callback),
		KHS_FN(1, "new", invoke_callback),
		KHS_FN(-2, "assert", assert_callback),
		KHS_FN(-3, "and?", and_callback),
		KHS_FN(-3, "or?", or_callback),
		KHS_FN(2, "<", less_callback),
		KHS_FN(2, ">", greater_callback), 
		KHS_FN(2, "=<=", less_eq_callback),
		KHS_FN(2, "=>=", greater_eq_callback),
		KHS_FN(3, "insert", insert_callback),
		KHS_FN(2, "union", union_callback),
		KHS_FN(-3, "cat", concat_callback),
		KHS_MANUAL_RELEASE_FN(2, "in?", in_callback),
		KHS_FN(1, "sizeof", sizeof_callback),
		KHS_FN(3, "replace", replace_callback),
		KHS_FN(-2, "eval", eval_callback),
		KHS_MANUAL_RELEASE_FN(2, "filter", filter_callback),
		KHS_FN(2, "push", push_callback),
		KHS_FN(2, "mod", mod_callback),
		KHS_FN(-2, "arrayof", arrayof_callback),
		KHS_FN(2, "construct-array", construct_array_callback),
		KHS_FN(1, "loop", loop_callback),
		KHS_FN(2, "find", find_callback),
		KHS_FN(2, "beginswith?", beginswith_callback),
		KHS_FN(2, "find-right", find_right_callback),
		KHS_FN(2, "endswith?", endswith_callback),
		KHS_FN(3, "substring", substring_callback),
		KHS_FN(2, "default", default_callback),
		KHS_FN(3, "str-replace", str_replace_callback),
		KHS_FN(1, "parse-number", parse_number_callback),
		KHS_FN(1, "as-string", as_string_callback),
		KHS_FN(1, "round", round_callback),
		KHS_FN(1, "is-int", is_int_callback),
		KHS_MANUAL_RELEASE_FN(2, "->", pipe_callback),
		KHS_FN(3, "try", try_callback),
		KHS_FN(-2, "error", error_callback),
		KHS_FN(1, "sort", sort_callback),
		KHS_FN(0, "random", random_callback),
		KHS_FN(3, "slice", slice_callback),
		KHS_FN(1, "pop", pop_callback),
		KHS_FN(1, "peek", peek_callback),
		KHS_FN(2, "apply", apply_callback),
		KHS_FN(2, "join", join_callback),
		KHS_FN(2, "map", map_callback),
		KHS_MANUAL_RELEASE_FN(-3, "forevery", forevery_callback),
		KHS_FN(1, "strip", strip_callback),
		KHS_FN(2, "split", split_callback),
		KHS_FN(1, "identity", identity_callback),
		KHS_FN(1, "typeof", typeof_callback),
		KHS_FN(2, "join-with", join_with_callback),
		KHS_FN(2, "repeat", repeat_callback),
		KHS_FN(-2, "max", max_callback),
		KHS_FN(2, "find-in", find_in_callback),
		KHS_FN(1, "floor", floor_callback),
		KHS_FN(1, "ceil", ceil_callback),
		KHS_FN(2, "first", first_callback),
		KHS_FN(2, "exists", exists_callback),
		KHS_FN(2, "all", all_callback)
	};
	else_tag = beryl_new_tag();
	elseif_tag = beryl_new_tag();
	catch_tag = beryl_new_tag();
	print_trace_tag = beryl_new_tag();
	beryl_set_var("else", sizeof("else") - 1, else_tag, true);
	beryl_set_var("elseif", sizeof("elseif") - 1, elseif_tag, true);
	beryl_set_var("catch", sizeof("catch") - 1, catch_tag, true);
	beryl_set_var("catch-log", sizeof("catch-log") - 1, print_trace_tag, true);
	for(size_t i = 0; i < LENOF(fns); i++) {
		bool ok = beryl_set_var(fns[i].name, fns[i].name_len, BERYL_EXT_FN(&fns[i]), true);
		if (!ok) {
			return false;
    }
	}
	beryl_set_var("false", sizeof("false") - 1, BERYL_FALSE, true);
	beryl_set_var("true", sizeof("true") - 1, BERYL_TRUE, true);
	beryl_set_var("null", sizeof("null") - 1, BERYL_NULL, true);
	beryl_set_var("max-int", sizeof("max-int") - 1, BERYL_NUMBER(KHS_NUM_MAX_INT), true);
	beryl_set_var("quote", sizeof("quote") - 1, BERYL_CONST_STR("\""), true);
	beryl_set_var("newline", sizeof("newline") - 1, BERYL_CONST_STR("\n"), true);
	beryl_set_var("tab", sizeof("tab") - 1, BERYL_CONST_STR("\t"), true);
	beryl_set_var("carriage-return", sizeof("carriage-return") - 1, BERYL_CONST_STR("\r"), true);
	DEF_FN("pairs", t, arrayof foreach-in t);
	DEF_FN("implies?", x y, (not x) or? y);
	DEF_FN("empty?", container, (sizeof container) == 0);
	return true;
}

