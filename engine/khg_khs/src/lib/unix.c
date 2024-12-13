#include "khg_khs/khs.h"
#include "khg_khs/lib/lib.h"
#include "khg_khs/util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <process.h>
typedef HINSTANCE khs_dyn_lib;
#define KHS_DL_IS_NULL(dl) ((dl) == NULL)
#define KHS_DL_OPEN(path) LoadLibraryA(path)
#define KHS_DL_PROC(dl, name) GetProcAddress(dl, name)
#define KHS_PLATFORM "windows"
#else
#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>
#include <regex.h>
typedef void *khs_dyn_lib;
#define KHS_DL_IS_NULL(dl) ((dl) == NULL)
#define KHS_DL_OPEN(path) dlopen(path, RTLD_NOW)
#define KHS_DL_PROC(dl, name) dlsym(dl, name)
#define KHS_PLATFORM "unix"
#endif

static khs_val PASS_INPUT_TAG;

static khs_val khs_load_dl_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;
	if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Expected string path as first argument");
	}
	khs_size path_len = KHS_LENOF(args[0]);
	char *c_path = khs_talloc(path_len + 1);
	if (c_path == NULL) {
		return KHS_ERR("Out of memory");
  }
	c_path[path_len] = '\0';
	memcpy(c_path, khs_get_raw_str((khs_val *) &args[0]), path_len);
	khs_dyn_lib dl = KHS_DL_OPEN(c_path);
	khs_tfree(c_path);
	if (KHS_DL_IS_NULL(dl)) {
		khs_blame_arg(args[0]);
#if defined(_WIN32) || defined(_WIN64)
		const char *err_msg = NULL;
#else
		const char *err_msg = dlerror();
		#endif
		if (err_msg != NULL) {
			khs_val err_str = khs_new_string(strlen(err_msg), err_msg);
			khs_blame_arg(err_str);
			khs_release(err_str);
		}
		return KHS_ERR("Unable to open dynamic library");
	}
	void *lib_load = KHS_DL_PROC(dl, "beryl_lib_load");
	if (lib_load == NULL) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Not a BerylScript library");
	}
	khs_val (*lib_load_fn_ptr)() = lib_load;
	return lib_load_fn_ptr();
}

static khs_val khs_getenv_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;
	if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Expected string as argument for 'getenv'");
	}
	khs_size len = KHS_LENOF(args[0]);
	char *c_name = khs_talloc(len + 1);
	if (c_name == NULL) {
		return KHS_ERR("Out of memory");
  }
	memcpy(c_name, khs_get_raw_str(&args[0]), len);
	c_name[len] = '\0';
	const char *env_var = getenv(c_name);
	khs_tfree(c_name);
	if (env_var == NULL) {
		return KHS_NULL;
  }
	khs_val res = khs_new_string(strlen(env_var), env_var);
	if (KHS_TYPEOF(res) == KHS_TYPE_NULL) {
		return KHS_ERR("Out of memory");
  }
	return res;
}

#if defined(_WIN32) || defined(_WIN64)
#else
static void khs_close_fd_pair(int p[2]) {
	close(p[0]);
	close(p[1]);
}
#endif

static int khs_p_spawn(const char *cmd, char **argv, int *return_code, const khs_val *pass) {
#if defined(_WIN32) || defined(_WIN64)
	if (pass != NULL) {
		return -3;
  }
	int res = _spawnvp(_P_WAIT, cmd, (const char * const *) argv);
	if (res == -1) {
		return -1;
  }
	*return_code = res;
	return 0;
#else	
	int host_to_sub[2];
	int perr = pipe(host_to_sub);
	if (perr) {
		return -1;
  }
	pid_t pid = fork();
	if (pid == -1) {
		khs_close_fd_pair(host_to_sub);
		return -1;
	}
  if (pid == 0) {
		close(host_to_sub[1]);
		if (pass != NULL) {
			dup2(host_to_sub[0], STDIN_FILENO);
    }
		close(host_to_sub[0]);
		execvp(cmd, argv);
		exit(127);
	} 
  else {
		close(host_to_sub[0]);
		if (pass != NULL) {
			assert(KHS_TYPEOF(*pass) == KHS_TYPE_STR);
			write(host_to_sub[1], khs_get_raw_str(pass), KHS_LENOF(*pass));
		}
		close(host_to_sub[1]);
		int wstatus;
		wait(&wstatus);
    if (WIFEXITED(wstatus)) {
			*return_code = WEXITSTATUS(wstatus);
			if (*return_code == 127) {
				return -4;
      }
		} 
    else {
			*return_code = -1;
    }
		return 0;
	}
#endif
}

static char **khs_convert_strs_to_cstrs(const khs_val *strs, size_t n_strs, bool null_end) {
	char **array;
  if (null_end) {
		array = khs_talloc((n_strs + 1) * sizeof(char *));
		if (array != NULL) {
			array[n_strs] = NULL;
    }
	} 
  else {
		array = khs_talloc(n_strs * sizeof(char *));
  }
	if (array == NULL) {
		return NULL;
  }
	for (size_t i = 0; i < n_strs; i++) {
		const char *str_src = khs_get_raw_str(&strs[i]);
		size_t len = KHS_LENOF(strs[i]);
		char *cstr = khs_alloc(len + 1);
		if (cstr == NULL) {
			for (ssize_t j = (ssize_t) i - 1; j >= 0; j--) {
				khs_free(array[j]);
			}
			khs_tfree(array);
			return NULL;
		}
		memcpy(cstr, str_src, len);
		cstr[len] = '\0';
		array[i] = cstr;
	}
	return array;
}

static khs_val khs_run_callback(const khs_val *args, khs_size n_args) {
	if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Expected command (string) as first argument for 'run'");
	}
	const khs_val *pass = NULL;
	for (khs_size i = 1; i < n_args; i++) { 
		if (khs_val_cmp(args[i], PASS_INPUT_TAG) == 0) {
			if (i + 1 != n_args - 1) {
				printf("%u, %u\n", (unsigned) i, (unsigned) n_args);
				return KHS_ERR("'pass-input' must be followed by exactly one argument");
			}
			if (KHS_TYPEOF(args[i+1]) != KHS_TYPE_STR) {
				khs_blame_arg(args[i+1]);
				return KHS_ERR("Can only pass strings to processes");
			}
			pass = &args[n_args - 1];
			assert(n_args > 2);
			n_args -= 2;
			break;
		}
		if (KHS_TYPEOF(args[i]) != KHS_TYPE_STR) {
			khs_blame_arg(args[i]);
			return KHS_ERR("Expected string as argument for 'run'");
		}
	}
	char **cmd = khs_convert_strs_to_cstrs(args, n_args, true);
	if (cmd == NULL) {
		return KHS_ERR("Out of memory");
  }
	int return_code = 0;
	int res = khs_p_spawn(cmd[0], cmd, &return_code, pass);
	for (size_t i = 0; i < n_args; i++) {
		khs_free(cmd[i]);
  }
	khs_tfree(cmd);
	switch (res) {
		case -1: {
			const char *cerr = strerror(errno);
			khs_val err_msg = khs_new_string(strlen(cerr), cerr);
			if (KHS_TYPEOF(err_msg) == KHS_TYPE_NULL) {
				return KHS_ERR("Cannot display system error; Out of memory");
      }
			else {
				return khs_str_as_err(err_msg);
      } }
		case -2:
			return KHS_ERR("Run is not supported on this platform");
		case -3:
			return KHS_ERR("'pass-input' is not supported on this platform");
		case -4:
			khs_blame_arg(args[0]);
			return KHS_ERR("Unable to run program");
	}
	return KHS_NUMBER(return_code);
}

static khs_val khs_random_string(khs_size len) {
#if defined(_WIN32) || defined(_WIN64)
	return KHS_ERR("'rands' is not supported on this platform");
#else
	khs_val str_val = khs_new_string(len, NULL);
	if (KHS_TYPEOF(str_val) == KHS_TYPE_NULL) {
		return KHS_ERR("Out of memory");
  }
	char *str = (char *) khs_get_raw_str(&str_val);
	FILE *f = fopen("/dev/urandom", "r");
	if (!f) {
		khs_release(str_val);
		return KHS_ERR("Internal error; Unable to open /dev/urandom");
	}
	size_t read_len = fread(str, sizeof(char), len, f);
	fclose(f);
	if (read_len != len) {
		khs_release(str_val);
		return KHS_ERR("Internal error; Unable to read from /dev/urandom");
	}
	return str_val;
#endif
}

static khs_val khs_rands_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;
	if (!khs_is_integer(args[0])) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Expected integer length as argument");
	}
	khs_float len = khs_as_num(args[0]);
	if (len < 0 || len > KHS_SIZE_MAX) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Random string length is out of range");
	}
	return khs_random_string((khs_size) len);
}

static khs_val khs_rands_hex(const khs_val *args, khs_size n_args) {
	(void)n_args;
	if (!khs_is_integer(args[0])) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Expected integer length as argument");
	}
	khs_float len = khs_as_num(args[0]);
	if (len < 0 || len > KHS_SIZE_MAX) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Random string length is out of range");
	}
	khs_size ilen = len;
	khs_val str = khs_random_string(ilen);
	if (KHS_TYPEOF(str) == KHS_TYPE_ERR) {
		return str;
  }
	char *str_bytes = (char *) khs_get_raw_str(&str);
	for (khs_size i = 0; i < ilen; i++) {
		unsigned char *byte = (unsigned char *) &str_bytes[i];
		unsigned char hex_val = *byte % 16;
		char hex_char = hex_val < 10 ? '0' + hex_val : 'a' + (hex_val - 10);
		str_bytes[i] = hex_char;
	}
	return str;
}

static khs_val khs_time_callback(const khs_val *args, khs_size n_args) {
	(void)args, (void)n_args;
	time_t t = time(NULL);
	if (t == -1) {
		return KHS_ERR("Time error");
	}
	return KHS_NUMBER(t);
}

static khs_val khs_convert_time_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;
	if (KHS_TYPEOF(args[0]) != KHS_TYPE_NUMBER) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Expected number as argument for 'format-time'");
	}
	time_t t = khs_as_num(args[0]);
	struct tm *time = localtime(&t);
	khs_val time_obj = khs_new_table(9, true);
	if (KHS_TYPEOF(time_obj) == KHS_TYPE_NULL) {
		return KHS_ERR("Out of memory");
  }
	khs_table_insert(&time_obj, KHS_CONST_STR("second"), KHS_NUMBER(time->tm_sec), false);
	khs_table_insert(&time_obj, KHS_CONST_STR("minute"), KHS_NUMBER(time->tm_min), false);
	khs_table_insert(&time_obj, KHS_CONST_STR("hour"), KHS_NUMBER(time->tm_hour), false);
	khs_table_insert(&time_obj, KHS_CONST_STR("day"), KHS_NUMBER(time->tm_mday), false);
	khs_table_insert(&time_obj, KHS_CONST_STR("month"), KHS_NUMBER(time->tm_mon), false);
	khs_table_insert(&time_obj, KHS_CONST_STR("year"), KHS_NUMBER(1900 + time->tm_year), false);
	khs_table_insert(&time_obj, KHS_CONST_STR("day-of-the-year"), KHS_NUMBER(time->tm_yday), false);
	khs_table_insert(&time_obj, KHS_CONST_STR("daylight-savings"), KHS_BOOL(time->tm_isdst), false);
	int weekday = time->tm_wday == 0 ? 6 : time->tm_wday - 1;
	khs_table_insert(&time_obj, KHS_CONST_STR("weekday"), KHS_NUMBER(weekday), false); 
	return time_obj;
}

static khs_val khs_get_time_callback(const khs_val *args, khs_size n_args) {
	(void)n_args;	
	KHS_REQ_NUM(second, "second");
	KHS_REQ_NUM(minute, "minute");
	KHS_REQ_NUM(hour, "hour");
	KHS_REQ_NUM(day, "day");
	KHS_REQ_NUM(month, "month");
	KHS_REQ_NUM(year, "year");
	khs_val daylight_savings = khs_call(args[0], &KHS_CONST_STR("daylight-savings"), 1, true);
	if (KHS_TYPEOF(daylight_savings) == KHS_TYPE_ERR) {
		return daylight_savings;
  }
	if (KHS_TYPEOF(daylight_savings) != KHS_TYPE_BOOL) {
		khs_blame_arg(args[0]);
		khs_blame_arg(args[1]);
		khs_release(daylight_savings);
		return KHS_ERR("Expected boolean given 'daylight-savings' for %0, got %1");
	}
	struct tm ts = { .tm_sec = khs_as_num(second), .tm_min = khs_as_num(minute), .tm_hour = khs_as_num(hour), .tm_mday = khs_as_num(day), .tm_mon = khs_as_num(month), .tm_year = khs_as_num(year) - 1900, .tm_isdst = khs_as_bool(daylight_savings) };
	time_t t = mktime(&ts);
	if(t == -1)	{
		khs_blame_arg(args[0]);
		return KHS_ERR("Invalid time, %0");
	}
	return KHS_NUMBER(t);
}

static khs_val khs_regex_callback(const khs_val *args, khs_size n_args) {
	(void)n_args, (void)args;
#if defined(_WIN32) || defined(_WIN64)
	return KHS_ERR("'regex' is not supported on this platform");
#else
	if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
		khs_blame_arg(args[0]);
		return KHS_ERR("Expected string (pattern) as first argument for 'regex'");
	}
	if (KHS_TYPEOF(args[1]) != KHS_TYPE_STR) {
		khs_blame_arg(args[1]);
		return KHS_ERR("Expected string as second argument for 'regex'");
	}
	size_t plen = KHS_LENOF(args[0]);
	size_t mlen = KHS_LENOF(args[1]);
	char *buff = khs_talloc(plen + 1 + mlen + 1);
	if (buff == NULL) {
		return KHS_ERR("Out of memory");
  }
	memcpy(buff, khs_get_raw_str(&args[0]), plen);
	buff[plen] = '\0';
	memcpy(buff + plen + 1, khs_get_raw_str(&args[1]), mlen);
	buff[plen + 1 + mlen] = '\0';
	regex_t reg;
	if (regcomp(&reg, buff, 0)) {
		khs_tfree(buff);
		khs_blame_arg(args[0]);
		return KHS_ERR("Regex error");
	}
#define MAX_MATCH 16
	static regmatch_t matches[MAX_MATCH];
	int res = regexec(&reg, buff + plen + 1, MAX_MATCH, matches, 0);
	if (res != 0) {
		matches[0] = (regmatch_t) { -1, -1 };
  }
	regfree(&reg);
	khs_tfree(buff);
	const char *match_str = khs_get_raw_str(&args[1]);
	khs_val res_array = khs_new_array(0, NULL, 1, false);
	if (KHS_TYPEOF(res_array) == KHS_TYPE_ERR) {
		return KHS_ERR("Out of memory");
  }
	for (size_t i = 0; i < MAX_MATCH; i++) {
		if (matches[i].rm_so == -1) {
			break;
    }
		size_t match_end = matches[0].rm_so + matches[i].rm_eo;
		khs_val str = khs_new_string(match_end - matches[i].rm_so, match_str + matches[i].rm_so);
		if (KHS_TYPEOF(str) == KHS_TYPE_ERR) {
			khs_release(res_array);
			return KHS_ERR("Out of memory");
		}
		bool ok = khs_array_push(&res_array, str);
		khs_release(str);
		if (!ok) {
			khs_release(res_array);
			return KHS_ERR("Out of memory");
		}
	}
	return res_array;
	#undef MAX_MATCH
	khs_tfree(buff);
#endif
}

bool khs_load_unix_lib() {
	static khs_external_fn fns[] = { KHS_FN(1, "load-dl", khs_load_dl_callback), KHS_FN(1, "getenv", khs_getenv_callback), KHS_FN(-2, "run", khs_run_callback), KHS_FN(1, "rands", khs_rands_callback), KHS_FN(1, "rand-hexs", khs_rands_hex), KHS_FN(0, "time", khs_time_callback), KHS_FN(1, "convert-time", khs_convert_time_callback), KHS_FN(1, "get-time", khs_get_time_callback), KHS_FN(2, "regex", khs_regex_callback) };
	for (size_t i = 0; i < KHS_UTIL_LENOF(fns); i++) {
		bool ok = khs_set_var(fns[i].name, fns[i].name_len, KHS_EXT_FN(&fns[i]), true);
		if (!ok) {
			return false;
    }
	}
	PASS_INPUT_TAG = khs_new_tag();
	khs_set_var("pass-input", sizeof("pass-input") - 1, PASS_INPUT_TAG, true);
	khs_set_var("platform", sizeof("platform") - 1, KHS_CONST_STR(KHS_PLATFORM), true);
	return true;
}

