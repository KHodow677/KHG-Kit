#pragma once

void default_error_func(const char *msg, void *userDefinedData);

extern void *user_defined_data;
typedef void (*error_func_type)(const char *, void *);
static error_func_type error_func = default_error_func;

void set_user_defined_data(void *data);
error_func_type set_error_func_callback(error_func_type newFunc);
