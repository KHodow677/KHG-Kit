#include "khg_khs/khs.h"
#include "khg_khs/lib/lib.h"
#include "khg_khs/util.h"

static khs_val khs_refcount_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;
	return KHS_NUMBER(khs_get_refcount(args[0]));
}

static khs_val khs_ptrof_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;
	const void *ptr = NULL;
	switch (KHS_TYPEOF(args[0])) {
		case KHS_TYPE_TABLE:
			ptr = args[0].val.table;
			break;
		case KHS_TYPE_ARRAY:
			ptr = khs_get_raw_array(args[0]);
			break;
	}
	return KHS_NUMBER((unsigned long long) ptr);
}

static khs_val khs_container_capacity_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;
	switch (KHS_TYPEOF(args[0])) {
		case KHS_TYPE_ARRAY:
			return KHS_NUMBER(khs_get_array_capacity(args[0]));
		default:
			return KHS_NUMBER(0);
	}
}

bool khs_load_debug_lib() {
	static khs_external_fn fns[] = { KHS_FN(1, "refcount", khs_refcount_callback), KHS_FN(1, "ptrof", khs_ptrof_callback), KHS_FN(1, "capof", khs_container_capacity_callback) };
	for (size_t i = 0; i < KHS_UTIL_LENOF(fns); i++) {
		bool ok = khs_set_var(fns[i].name, fns[i].name_len, KHS_EXT_FN(&fns[i]), true);
		if (!ok) {
			return false;
    }
	}
	khs_set_var("max-refcount", sizeof("max-refcount") - 1, KHS_NUMBER(KHS_REFC_MAX), true);
	khs_set_var("valsize", sizeof("valsize") - 1, KHS_NUMBER(sizeof(khs_val)), true);
	return true;
}

