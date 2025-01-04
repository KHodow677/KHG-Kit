#include "khg_khs/io.h"
#include "khg_khs/khs.h"
#include "khg_khs/lib/lib.h"
#include "khg_khs/util.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static khs_val READ_ALL_TAG = KHS_NULL;

static char *khs_as_path(const char *str, size_t len) {
  char *res = khs_talloc(len + 1);
  if (res == NULL) {
    return NULL;
  }
  memcpy(res, str, len);
  res[len] = '\0';
  return res;
}

static void khs_free_path(char *path) {
  khs_tfree(path);
}

static khs_val khs_read_callback(const khs_val *args, khs_size n_args) {
  (void) n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected string path as argument for 'read'");
  }
  
  khs_size path_len = KHS_LENOF(args[0]);
  char *c_path = khs_as_path(khs_get_raw_str(&args[0]), path_len);
  if (c_path == NULL) {
    return KHS_ERR("Out of memory");
  }
  unsigned int file_len;
  char *file_contents = khs_load_file(c_path, &file_len);
  khs_free_path(c_path);
  if (file_contents == NULL) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Unable to read from file");
  }
  if (file_len > KHS_SIZE_MAX) {
    khs_free(file_contents);
    return KHS_ERR("File too large");
  }
  khs_val str_res = khs_new_string(file_len, file_contents);
  khs_free(file_contents);
  if (KHS_TYPEOF(str_res) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  return str_res;
}

static khs_val khs_print_vals_callback(const khs_val *args, khs_size n_args) {
  for (khs_size i = 0; i < n_args; i++) {
    khs_print_val(stdout, args[i]);
    if (i != n_args - 1) {
      putchar(' ');
    }
  }
  putchar('\n');
  return KHS_NULL;
}

static khs_val khs_write_val_to_file(khs_val path, khs_val content, const char *mode) {
  assert(KHS_TYPEOF(path) == KHS_TYPE_STR);
  assert(KHS_TYPEOF(content) == KHS_TYPE_STR);
  
  char *c_path = khs_as_path(khs_get_raw_str(&path), KHS_LENOF(path));
  if(c_path == NULL)
    return KHS_ERR("Out of memory");
  
  FILE *f = fopen(c_path, mode);
  khs_free_path(c_path);
  if(f == NULL) {
    khs_blame_arg(path);
    return KHS_ERR("Unable to open file");
  }
  
  size_t res = fwrite(khs_get_raw_str(&content), sizeof(char), KHS_LENOF(content), f);
  fclose(f);
  
  if(res != KHS_LENOF(content)) {
    khs_blame_arg(path);
    return KHS_ERR("Error when writing to file");
  }
  return KHS_NULL;
}

static khs_val khs_write_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected file path as first argument for 'write'");
  }
  if (KHS_TYPEOF(args[1]) != KHS_TYPE_STR) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Expected string as second argument for 'write'");
  }
  return khs_write_val_to_file(args[0], args[1], "w");
}

static khs_val khs_print_exactly_callback(const khs_val *args, khs_size n_args) {
  for (khs_size i = 0; i < n_args; i++) {
    khs_print_val(stdout, args[i]);
  }
  return KHS_NULL;
}

static khs_val khs_input_callback(const khs_val *args, khs_size n_args) {
  bool stop_at_newline = true;
  if (n_args >= 1 && khs_val_cmp(args[0], READ_ALL_TAG) == 0) {
    args++;
    n_args--;
    stop_at_newline = false;
  }
  for (khs_size i = 0; i < n_args; i++) {
    khs_print_val(stdout, args[i]);
    if (i != n_args - 1) {
      putchar(' ');
    }
  }
  khs_size line_buff_size = 256;
  char *line_buff = khs_talloc(line_buff_size);
  khs_size n_read = 0;
  int i;
  while ((i = getchar()) != EOF) {
    char c = i;
    if (c == '\n' && stop_at_newline) {
      break;
    }
    if (n_read == line_buff_size) {
      line_buff_size = line_buff_size * 3 / 2 + 1;
      if (line_buff_size <= n_read) {
        khs_tfree(line_buff);
        return KHS_ERR("Out of memory");
      }
      void *newp = khs_realloc(line_buff, line_buff_size);
      if (newp == NULL) {
        khs_tfree(line_buff);
        return KHS_ERR("Out of memory");
      }
      line_buff = newp;
    }
    line_buff[n_read++] = c;
  }
  khs_val res_str = khs_new_string(n_read, line_buff);
  khs_tfree(line_buff);
  if (KHS_TYPEOF(res_str) == KHS_TYPE_NULL) {
    return KHS_ERR("Out of memory");
  }
  return res_str;
}

static khs_val khs_append_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected file path as first argument for 'append'");
  }
  if (KHS_TYPEOF(args[1]) != KHS_TYPE_STR) {
    khs_blame_arg(args[1]);
    return KHS_ERR("Expected string as second argument for 'append'");
  }
  return khs_write_val_to_file(args[0], args[1], "a");  
}

static khs_val khs_file_exists_callback(const khs_val *args, khs_size n_args) {
  (void)n_args;
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected file path (string) as argument for 'file-exists?'");
  }
  char *path = khs_as_path(khs_get_raw_str(&args[0]), KHS_LENOF(args[0]));
  if (path == NULL) {
    return KHS_ERR("Out of memory");
  }
  FILE *f = fopen(path, "r");
  khs_free_path(path);
  if(f == NULL) {
    return KHS_BOOL(0);
  }
  fclose(f);
  return KHS_BOOL(1);
}

static khs_val khs_printf_callback(const khs_val *args, khs_size n_args) {
  if (KHS_TYPEOF(args[0]) != KHS_TYPE_STR) {
    khs_blame_arg(args[0]);
    return KHS_ERR("Expected string as first argument for 'printf', got '%0'");
  }
  khs_size n_format_vals = n_args - 1;
  if (n_format_vals > 10) {
    khs_blame_arg(KHS_NUMBER(n_format_vals));
    return KHS_ERR("printf accepts at most 10 values to interpolate (Got %0)");
  }
  const char *str = khs_get_raw_str(&args[0]);
  khs_size strlen = KHS_LENOF(args[0]);
  khs_vals_printf(stdout, str, strlen, args + 1, n_args - 1);
  return KHS_NULL;
}

bool khs_load_io_lib() {
  static khs_external_fn fns[] = {
    KHS_FN(1, "read", khs_read_callback),
    KHS_FN(-2, "print", khs_print_vals_callback),
    KHS_FN(2, "write", khs_write_callback),
    KHS_FN(-2, "print-exactly", khs_print_exactly_callback),
    KHS_FN(-1, "input", khs_input_callback),
    KHS_FN(2, "append", khs_append_callback),
    KHS_FN(1, "file-exists?", khs_file_exists_callback),
    KHS_FN(-2, "printf", khs_printf_callback),
  };
  for (size_t i = 0; i < KHS_UTIL_LENOF(fns); i++) {
    bool ok = khs_set_var(fns[i].name, fns[i].name_len, KHS_EXT_FN(&fns[i]), true);
    if (!ok) {
      return false;
    }
  }
  khs_set_var("path-separator", sizeof("path-separator") - 1, KHS_CONST_STR(KHS_PATH_SEPARATOR), true);
  READ_ALL_TAG = khs_new_tag();
  khs_set_var("read-all", sizeof("read-all") - 1, READ_ALL_TAG, true);
  return true;
}

