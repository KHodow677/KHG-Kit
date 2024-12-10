#pragma once

#include <stdint.h>
#include <stddef.h>

enum {
  GSC_OK,
  GSC_ERROR,
  GSC_YIELD,
  GSC_NOT_FOUND,
  GSC_OUT_OF_MEMORY
};

#define GSC_TYPES(X)\
	X(UNDEFINED)\
	X(STRING)\
	X(INTERNED_STRING)\
	X(INTEGER)\
	X(BOOLEAN)\
	X(FLOAT)\
	X(VECTOR)\
	X(FUNCTIO\
	X(OBJECT)\
	X(REFEREN\
	X(THREAD)

#define GSC_TYPES_STRINGS(TYPE) #TYPE, static const char *gsc_type_names[] = { GSC_TYPES(GSC_TYPES_STRINGS) NULL };
#define GSC_TYPES_ENUM(TYPE) GSC_TYPE_##TYPE, enum { GSC_TYPES(GSC_TYPES_ENUM) GSC_TYPE_MAX };

#define GSC_COMPILE_FLAG_NONE (0)
#define GSC_COMPILE_FLAG_PRINT_EXPRESSION (1)

typedef struct gsc_Context gsc_Context;
typedef int (*gsc_Function)(gsc_Context *);
typedef struct gsc_Object gsc_Object;

typedef struct {
  void *(*allocate_memory)(void *ctx, int size);
  void (*free_memory)(void *ctx, void *ptr);
  const char *(*read_file)(void *ctx, const char *filename, int *status);
  void *userdata;
  int verbose;
  int main_memory_size;
  int temp_memory_size;
  int string_table_memory_size;
  const char *default_self;
  int max_threads;
} gsc_CreateOptions;

typedef struct {
	const char *name;
	gsc_Function function;
} gsc_FunctionEntry;

typedef struct {
	const char *name;
	gsc_Function getter;
	gsc_Function setter;
} gsc_FieldEntry;

gsc_Context *gsc_create(gsc_CreateOptions options);
void gsc_destroy(gsc_Context *ctx);
void gsc_error(gsc_Context *ctx, const char *fmt, ...);

int gsc_link(gsc_Context *ctx);

int gsc_compile(gsc_Context *ctx, const char *filename, int flags);
const char *gsc_next_compile_dependency(gsc_Context *ctx);
void *gsc_temp_alloc(gsc_Context *ctx, int size);

int gsc_update(gsc_Context *ctx, float dt);
int gsc_call(gsc_Context *ctx, const char *file, const char *function, int nargs);
int gsc_call_method(gsc_Context *ctx, const char *file, const char *function, int nargs);

void gsc_object_set_debug_info(gsc_Context *ctx, void *object, const char *file, const char *function, int line);
	
int gsc_register_string(gsc_Context *ctx, const char *s);
const char *gsc_string(gsc_Context *ctx, int index);

void gsc_register_function(gsc_Context *ctx, const char *file, const char *name, gsc_Function);

void gsc_object_set_field(gsc_Context *ctx, int obj_index, const char *name);
void gsc_object_get_field(gsc_Context *ctx, int obj_index, const char *name);
const char *gsc_object_get_tag(gsc_Context *ctx, int obj_index);

int gsc_top(gsc_Context *ctx);
int gsc_type(gsc_Context *ctx, int index);
void gsc_push(gsc_Context *ctx, void *value);
int gsc_push_object(gsc_Context *ctx, void *value);
void gsc_pop(gsc_Context *ctx, int count);

void *gsc_allocate_object(gsc_Context *ctx);

int gsc_add_object(gsc_Context *ctx);
int gsc_add_tagged_object(gsc_Context *ctx, const char *tag);
int gsc_object_get_proxy(gsc_Context *ctx, int obj_index);
void gsc_object_set_proxy(gsc_Context *ctx, int obj_index, int proxy_index);

void *gsc_object_get_userdata(gsc_Context *ctx, int obj_index);
void gsc_object_set_userdata(gsc_Context *ctx, int obj_index, void *userdata);

void gsc_add_int(gsc_Context *ctx, int64_t value);
void gsc_add_float(gsc_Context *ctx, float value);
void gsc_add_string(gsc_Context *ctx, const char *value);
void gsc_add_vec3(gsc_Context *ctx, float *value);
void gsc_add_function(gsc_Context *ctx, gsc_Function value);
void gsc_add_bool(gsc_Context *state, int cond);

int64_t gsc_to_int(gsc_Context *ctx, int index);
float gsc_to_float(gsc_Context *ctx, int index);
const char *gsc_to_string(gsc_Context *ctx, int index);
	
int64_t gsc_get_int(gsc_Context *ctx, int index);
int gsc_get_bool(gsc_Context *ctx, int index);
float gsc_get_float(gsc_Context *ctx, int index);
const char *gsc_get_string(gsc_Context *ctx, int index);
void gsc_get_vec3(gsc_Context *ctx, int index, float *v);
int gsc_get_object(gsc_Context *ctx, int index);
int gsc_get_type(gsc_Context *ctx, int index);
int gsc_numargs(gsc_Context *ctx);
int gsc_arg(gsc_Context *ctx, int index);

void* gsc_get_ptr(gsc_Context *ctx, int index);

int gsc_get_global(gsc_Context *ctx, const char *name);
void gsc_set_global(gsc_Context *ctx, const char *name);

void *gsc_get_internal_pointer(gsc_Context *ctx, const char *tag);
