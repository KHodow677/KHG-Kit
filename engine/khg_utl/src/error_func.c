#include "khg_utl/error_func.h"
#include <stdio.h>

void *utl_user_defined_data = 0;

void utl_default_error_func(const char *msg, void *user_defined_data) {
  printf("KHG Error: %s\n", msg);
}

void utl_set_user_defined_data(void *data) {
  utl_user_defined_data = data;
}

utl_error_func_type utl_set_error_func_callback(utl_error_func_type new_func) {
  utl_error_func_type a = utl_error_func;
  utl_error_func = new_func;
  return a;
}
