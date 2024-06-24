#include "khg_utils/error_func.h"
#include <stdio.h>

void *user_defined_data = 0;

void default_error_func(const char *msg, void *userDefinedData) {
  printf("KHG error: %s\n", msg);
}

void set_user_defined_data(void *data) {
  user_defined_data = data;
}

error_func_type set_error_func_callback(error_func_type newFunc) {
  error_func_type a = error_func;
  error_func = newFunc;
  return a;
}
