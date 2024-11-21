#include "khg_utl/map.h"
#include "khg_utl/vector.h"
#include <stdbool.h>

typedef enum {
  UTL_JSON_NULL,
  UTL_JSON_BOOL,
  UTL_JSON_NUMBER,
  UTL_JSON_STRING,
  UTL_JSON_ARRAY,
  UTL_JSON_OBJECT
} utl_json_type;

typedef union {
  bool bool_val;
  double number_val;
  char *string_val;
  utl_vector *array_val;
  utl_map *object_val;
} utl_json_value;

typedef struct {
  utl_json_type type;
  utl_json_value value;
} utl_json_element;

typedef bool (*utl_json_predicate)(const utl_json_element *, void *);
typedef utl_json_element *(*utl_json_map_function)(const utl_json_element *, void *);
typedef void *(*utl_json_reduce_function)(const utl_json_element *, void *, void *);

typedef struct {
  int a;
} utl_json_parse_options;

typedef enum {
  UTL_JSON_TOKEN_OBJECT_START,
  UTL_JSON_TOKEN_OBJECT_END,
  UTL_JSON_TOKEN_ARRAY_START,
  UTL_JSON_TOKEN_ARRAY_END,
  UTL_JSON_TOKEN_STRING,
  UTL_JSON_TOKEN_NUMBER,
  UTL_JSON_TOKEN_BOOLEAN,
  UTL_JSON_TOKEN_NULL,
  UTL_JSON_TOKEN_COLON,
  UTL_JSON_TOKEN_COMMA,
  UTL_JSON_TOKEN_EOF,
  UTL_JSON_TOKEN_ERROR,
} utl_json_token_type;

typedef struct {
  utl_json_token_type type;
  char* value;
} utl_json_token;

typedef struct {
  char* input;
  size_t input_len;
  size_t position;
  utl_json_token current_token;
} utl_json_parser_state;

utl_json_element *utl_json_parse(const char *json_str);
utl_json_element *utl_json_read_from_file(const char *filename);
utl_json_element *utl_json_get_element(const utl_json_element *element, const char *key_or_index);
utl_json_element *utl_json_query(const utl_json_element *element, const char *query);
utl_json_element *utl_json_merge(const utl_json_element *element1, const utl_json_element *element2);
utl_json_element *utl_json_deep_copy(const utl_json_element *element);
utl_json_element *utl_json_parse_with_options(const char *json_str, utl_json_parse_options options);
utl_json_element *utl_json_find(const utl_json_element *element, utl_json_predicate predicate, void *user_data);
utl_json_element *utl_json_filter(const utl_json_element *array, utl_json_predicate predicate, void *user_data);
utl_json_element *utl_json_map(const utl_json_element *array, utl_json_map_function map_func, void *user_data);
utl_json_element *utl_json_create(utl_json_type type);
utl_json_element *utl_json_clone(const utl_json_element *element);

char *utl_json_serialize(const utl_json_element *element);
char *utl_json_format(const utl_json_element *element);
char *utl_json_generate_schema(const utl_json_element* element);

char **utl_json_to_string_array(const utl_json_element *array, size_t *length);
char **utl_json_get_keys(const utl_json_element *object, size_t *num_keys);

bool utl_json_write_to_file(const utl_json_element *element, const char *filename);
bool utl_json_set_element(utl_json_element *element, const char *key_or_index, utl_json_element *new_element);
bool utl_json_remove_element(utl_json_element *element, const char *key_or_index);
bool utl_json_validate(const utl_json_element *element, const char *schema_json);
bool utl_json_compare(const utl_json_element *element1, const utl_json_element *element2);
bool utl_json_add_to_array(utl_json_element *element1, utl_json_element *element2);
bool utl_json_add_to_object(utl_json_element *object, const char* key, utl_json_element *value);

void *utl_json_convert(const utl_json_element *element, utl_json_type type);
void *utl_json_reduce(const utl_json_element *array, utl_json_reduce_function reduce_func, void *initial_value, void *user_data);
size_t utl_json_array_size(const utl_json_element *array);
size_t utl_json_object_size(const utl_json_element *object);

utl_json_type utl_json_type_of_element(const utl_json_element *element);

void utl_json_print(const utl_json_element *element);
void utl_json_deallocate(utl_json_element *element);
