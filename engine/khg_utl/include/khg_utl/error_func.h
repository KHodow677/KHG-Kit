#pragma once

void utl_default_error_func(const char *msg, void *user_defined_data);

extern void *utl_user_defined_data;
typedef void (*utl_error_func_type)(const char *, void *);
static utl_error_func_type utl_error_func = utl_default_error_func;

void utl_set_user_defined_data(void *data);
utl_error_func_type utl_set_error_func_callback(utl_error_func_type new_func);

