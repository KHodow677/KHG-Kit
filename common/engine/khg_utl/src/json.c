#include "khg_utl/json.h"
#include "khg_utl/error_func.h"
#include "khg_utl/file_reader.h"
#include "khg_utl/map.h"
#include "khg_utl/string.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

static utl_json_element *utl_parse_array(utl_json_parser_state *state);
static utl_json_element *utl_parse_string(utl_json_parser_state *state);
static utl_json_element *utl_parse_number(utl_json_parser_state *state);
static utl_json_element *utl_parse_null(utl_json_parser_state *state);
static utl_json_element *utl_parse_boolean(utl_json_parser_state *state);
static utl_json_element *utl_parser_internal(utl_json_parser_state *state);
static utl_json_element *utl_parse_object(utl_json_parser_state *state);
static void utl_json_serialize_internal(const utl_json_element *element, utl_string *str);
static bool utl_json_find_in_object(const utl_json_element *object, utl_json_predicate predicate, void *user_data, utl_json_element **found_element);
static bool utl_json_find_in_array(const utl_json_element *array, utl_json_predicate predicate, void *user_data, utl_json_element **found_element);
static void utl_json_format_internal(const utl_json_element *element, utl_string *str, int indent);

static void utl_next_token(utl_json_parser_state *state) {
  while (isspace((unsigned char)state->input[state->position])) {
    state->position++;
  }
  if (state->input[state->position] == '\0') {
    state->current_token.type = UTL_JSON_TOKEN_EOF;
    return;
  }
  char currentChar = state->input[state->position];
  switch (currentChar) {
    case '{':
      state->current_token.type = UTL_JSON_TOKEN_OBJECT_START;
      break;
    case '}':
      state->current_token.type = UTL_JSON_TOKEN_OBJECT_END;
      break;
    case '[':
      state->current_token.type = UTL_JSON_TOKEN_ARRAY_START;
      break;
    case ']':
      state->current_token.type = UTL_JSON_TOKEN_ARRAY_END;
      break;
    case '\"':
      state->current_token.type = UTL_JSON_TOKEN_STRING;
      break;
    case ':':
      state->current_token.type = UTL_JSON_TOKEN_COLON;
      break;
    case ',':
      state->current_token.type = UTL_JSON_TOKEN_COMMA;
      break;
    case '-':
    case '+':
      state->current_token.type = UTL_JSON_TOKEN_NUMBER;
      break;
    default:
      if (isdigit((unsigned char)currentChar)) {
        state->current_token.type = UTL_JSON_TOKEN_NUMBER;
      } 
      else if (strncmp(state->input + state->position, "true", 4) == 0) {
        state->current_token.type = UTL_JSON_TOKEN_BOOLEAN;
      } 
      else if (strncmp(state->input + state->position, "false", 5) == 0) {
        state->current_token.type = UTL_JSON_TOKEN_BOOLEAN;
      } 
      else if (strncmp(state->input + state->position, "null", 4) == 0) {
        state->current_token.type = UTL_JSON_TOKEN_NULL;
      } 
      else {
        state->current_token.type = UTL_JSON_TOKEN_ERROR;
        utl_error_func("Unexpected token detected", utl_user_defined_data);
      }
      break;
  }
  state->position++;
}

static utl_json_element *utl_parse_array(utl_json_parser_state *state) {
  if (state->current_token.type != UTL_JSON_TOKEN_ARRAY_START) {
    utl_error_func("Expected start of array", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *array_element = utl_json_create(UTL_JSON_ARRAY);
  if (!array_element) {
    utl_error_func("Memory allocation failed for array", utl_user_defined_data);
    return NULL;
  }
  utl_next_token(state);
  while (state->current_token.type != UTL_JSON_TOKEN_ARRAY_END) {
    if (state->current_token.type == UTL_JSON_TOKEN_COMMA) {
      utl_next_token(state);
    }
    else if (state->current_token.type == UTL_JSON_TOKEN_EOF) {
      utl_error_func("Unexpected end of input in array", utl_user_defined_data);
      utl_json_deallocate(array_element);
      return NULL;
    }
    utl_json_element *element = utl_parser_internal(state);
    if (!element) {
      utl_error_func("Failed to parse element in array", utl_user_defined_data);
      utl_json_deallocate(array_element);
      return NULL;
    }
    utl_vector_push_back(array_element->value.array_val, &element);
    utl_next_token(state);
  }
  return array_element;
}

static utl_json_element *utl_parse_string(utl_json_parser_state *state) {
  if (state->current_token.type != UTL_JSON_TOKEN_STRING) {
    utl_error_func("Expected string token", utl_user_defined_data);
    return NULL;
  }
  size_t start = state->position;
  while (state->input[state->position] != '\"' && state->input[state->position] != '\0') {
    if (state->input[state->position] == '\0') {
      utl_error_func("Unterminated string", utl_user_defined_data);
      return NULL;
    }
    state->position++;
  }
  if (state->input[state->position] == '\0') {
    utl_error_func("Unterminated string", utl_user_defined_data);
    return NULL;
  }
  size_t length = state->position - start;
  char *str_content = (char *)malloc(length + 1);
  if (!str_content) {
    utl_error_func("Memory allocation failed for string", utl_user_defined_data);
    return NULL;
  }
  strncpy(str_content, state->input + start, length);
  str_content[length] = '\0';
  state->position++;
  utl_json_element *element = utl_json_create(UTL_JSON_STRING);
  if (!element) {
    utl_error_func("Failed to create JSON string element", utl_user_defined_data);
    free(str_content);
    return NULL;
  }
  element->value.string_val = str_content;
  return element;
}

static utl_json_element *utl_parse_number(utl_json_parser_state *state) {
  if (state->current_token.type != UTL_JSON_TOKEN_NUMBER) {
    utl_error_func("Expected number token", utl_user_defined_data);
    return NULL;
  }
  size_t start = state->position - 1;
  while (isdigit((unsigned char)state->input[state->position]) || state->input[state->position] == '.' || state->input[state->position] == 'e' || state->input[state->position] == 'E' || (state->position != start && (state->input[state->position] == '+' || state->input[state->position] == '-'))) {
    state->position++;
  }
  size_t length = state->position - start;
  char *number_str = (char *)malloc(length + 1);
  if (!number_str) {
    utl_error_func("Memory allocation failed for number string", utl_user_defined_data);
    return NULL;
  }
  strncpy(number_str, state->input + start, length);
  number_str[length] = '\0';
  char *endptr;
  double number_double = strtod(number_str, &endptr);
  if (endptr == number_str) {
    utl_error_func("Invalid number format", utl_user_defined_data);
    free(number_str);
    return NULL;
  }
  free(number_str);
  utl_json_element *element = utl_json_create(UTL_JSON_NUMBER);
  if (!element) {
    utl_error_func("Failed to create JSON number element", utl_user_defined_data);
    return NULL;
  }
  element->value.number_val = number_double;
  return element;
}

static utl_json_element *utl_parse_null(utl_json_parser_state *state) {
  if (state->current_token.type != UTL_JSON_TOKEN_NULL) {
    utl_error_func("Expected null token", utl_user_defined_data);
    return NULL;
  }
  state->position += 4;
  utl_json_element *element = utl_json_create(UTL_JSON_NULL);
  if (!element) {
    utl_error_func("Failed to create JSON null element", utl_user_defined_data);
    return NULL;
  }
  return element;
}

static utl_json_element *utl_parse_boolean(utl_json_parser_state *state) {
  if (state->current_token.type != UTL_JSON_TOKEN_BOOLEAN) {
    utl_error_func("Expected boolean token", utl_user_defined_data);
    return NULL;
  }
  size_t start = state->position - 1;
  bool boolean_value;
  if (strncmp(state->input + start, "true", 4) == 0) {
    state->current_token.type = UTL_JSON_TOKEN_BOOLEAN;
    state->position += 3;
    boolean_value = true;
  } 
  else if (strncmp(state->input + start, "false", 5) == 0) {
    state->current_token.type = UTL_JSON_TOKEN_BOOLEAN;
    state->position += 4;
    boolean_value = false;
  } 
  else {
    utl_error_func("Invalid boolean format", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *element = utl_json_create(UTL_JSON_BOOL);
  if (!element) {
    utl_error_func("Failed to create JSON boolean element", utl_user_defined_data);
    return NULL;
  }
  element->type = UTL_JSON_BOOL;
  element->value.bool_val = boolean_value;
  return element;
}

static utl_json_element *utl_parser_internal(utl_json_parser_state *state) {
  switch (state->current_token.type) {
    case UTL_JSON_TOKEN_OBJECT_START:
      return utl_parse_object(state);
    case UTL_JSON_TOKEN_ARRAY_START:
      return utl_parse_array(state);
    case UTL_JSON_TOKEN_STRING:
      return utl_parse_string(state);
    case UTL_JSON_TOKEN_NUMBER:
      return utl_parse_number(state);
    case UTL_JSON_TOKEN_BOOLEAN:
      return utl_parse_boolean(state);
    case UTL_JSON_TOKEN_NULL:
      return utl_parse_null(state);
    default:
      utl_error_func("Unexpected token", utl_user_defined_data);
      return NULL;
  }
}

static utl_json_element *utl_parse_object(utl_json_parser_state *state) {
  if (state->current_token.type != UTL_JSON_TOKEN_OBJECT_START) {
    utl_error_func("Expected start of object", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *object_element = utl_json_create(UTL_JSON_OBJECT);
  if (!object_element) {
    utl_error_func("Failed to create JSON object element", utl_user_defined_data);
    return NULL;
  }
  utl_next_token(state);
  while (state->current_token.type != UTL_JSON_TOKEN_OBJECT_END) {
    if (state->current_token.type != UTL_JSON_TOKEN_STRING) {
      utl_error_func("Expected string as key in object", utl_user_defined_data);
      utl_json_deallocate(object_element);
      return NULL;
    }
    char *key = utl_parse_string(state)->value.string_val;
    utl_next_token(state);
    if (state->current_token.type != UTL_JSON_TOKEN_COLON) {
      utl_error_func("Expected colon after key in object", utl_user_defined_data);
      free(key);
      utl_json_deallocate(object_element);
      return NULL;
    }
    utl_next_token(state);
    utl_json_element *value = utl_parser_internal(state);
    if (!value) {
      free(key);
      utl_json_deallocate(object_element);
      return NULL;
    }
    utl_map_insert(object_element->value.object_val, key, value);
    utl_next_token(state);
    if (state->current_token.type == UTL_JSON_TOKEN_COMMA) {
      utl_next_token(state);
    }
  }
  return object_element;
}

static void print_indent(int indent) {
  for (int i = 0; i < indent; i++) {
    printf(" ");
  }
}

static void json_print_internal(const utl_json_element *element, int indent) {
  if (!element) {
    printf("null");
    return;
  }
  switch (element->type) {
    case UTL_JSON_OBJECT:
      printf("{\n");
      utl_map_iterator it = utl_map_begin(element->value.object_val);
      utl_map_iterator end = utl_map_end(element->value.object_val);
      bool first = true;
      while (it.node != end.node) {
          if (!first) {
              printf(",\n");
          }
          print_indent(indent + 2);
          printf("\"%s\": ", (char *)it.node->key);
          json_print_internal((utl_json_element *)it.node->value, indent + 2);
          first = false;
          utl_map_iterator_increment(&it);
      }
      if (!first) {
          printf("\n");
      }
      print_indent(indent - 2);
      printf("}");
      break;
    case UTL_JSON_ARRAY:
      printf("[\n");
      for (size_t i = 0; i < utl_vector_size(element->value.array_val); ++i) {
          print_indent(indent + 2);
          json_print_internal(*(utl_json_element **)utl_vector_at(element->value.array_val, i), indent + 2);
          if (i < utl_vector_size(element->value.array_val) - 1) {
              printf(",");
          }
          printf("\n");
      }
      print_indent(indent - 2);
      printf("]");
      break;
    case UTL_JSON_STRING:
      printf("\"%s\"", element->value.string_val);
      break;
    case UTL_JSON_NUMBER:
      if (floor(element->value.number_val) == element->value.number_val) {
        printf("%d", (int)element->value.number_val);
      } 
      else {
        printf("%.2f", element->value.number_val);
      }
      break;
    case UTL_JSON_BOOL:
      printf(element->value.bool_val ? "true" : "false");
      break;
    case UTL_JSON_NULL:
      printf("null");
      break;
    default:
      printf("Unknown type");
  }
}

static char **split_query(const char *query, int *count) {
  static char *tokens[256];
  int i = 0;
  if (!query) {
    utl_error_func("Null query provided", utl_user_defined_data);
    return NULL;
  }
  char *tempQuery = utl_string_strdup(query);
  if (!tempQuery) {
    utl_error_func("Memory allocation failed for temporary query", utl_user_defined_data);
    return NULL;
  }
  char *token = strtok(tempQuery, ".");
  while (token != NULL && i < 256) {
    tokens[i++] = utl_string_strdup(token); 
    if (!tokens[i - 1]) {
      utl_error_func("Memory allocation failed for token", utl_user_defined_data);
      free(tempQuery);
      return NULL;
    }
    token = strtok(NULL, ".");
  }
  *count = i;
  free(tempQuery);
  return tokens;
}

void utl_json_deallocate(utl_json_element *element) {
  if (!element) {
    return;
  }
  switch (element->type) {
    case UTL_JSON_STRING:
      if (element->value.string_val) {
        free(element->value.string_val);
      } 
      break;
    case UTL_JSON_ARRAY:
      if (element->value.array_val) {
        for (size_t i = 0; i < utl_vector_size(element->value.array_val); ++i) {
          utl_json_element* e = *(utl_json_element **)utl_vector_at(element->value.array_val, i);
          utl_json_deallocate(e);
        }
        utl_vector_deallocate(element->value.array_val);
      } 
      break;
    case UTL_JSON_OBJECT:
      if (element->value.object_val) {
        utl_map_deallocate(element->value.object_val);
      } 
      break;
    default:
      break;
  }
  free(element);
}

static bool utl_json_find_in_object(const utl_json_element *object, utl_json_predicate predicate, void *user_data, utl_json_element **found_element) {
  if (!object || object->type != UTL_JSON_OBJECT) {
    utl_error_func("The provided element is null or not a JSON object", utl_user_defined_data);
    return false;
  }
  if (!predicate) {
    utl_error_func("Predicate function is null", utl_user_defined_data);
    return false;
  }
  utl_map_iterator it = utl_map_begin(object->value.object_val);
  utl_map_iterator end = utl_map_end(object->value.object_val);
  while (it.node != end.node) {
    utl_json_element *current_element = (utl_json_element *)it.node->value;
    if (predicate(current_element, user_data)) {
      *found_element = current_element;
      return true;
    }
    utl_map_iterator_increment(&it);
  }
  return false;
}

static bool utl_json_find_in_array(const utl_json_element *array, utl_json_predicate predicate, void *user_data, utl_json_element **found_element) {
  if (!array || array->type != UTL_JSON_ARRAY) {
    utl_error_func("The provided element is null or not a JSON array", utl_user_defined_data);
    return false;
  }
  if (!predicate) {
    utl_error_func("Predicate function is null", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < utl_vector_size(array->value.array_val); ++i) {
    utl_json_element *current_element = *(utl_json_element **)utl_vector_at(array->value.array_val, i);
    if (predicate(current_element, user_data)) {
      *found_element = current_element;
      return true;
    }
  }
  return false;
}

static void json_element_deallocator(void *data) {
  if (!data) {
    return;
  }
  utl_json_element *element = (utl_json_element *)data;
  utl_json_deallocate(element);
}

static int compare_strings_json(const utl_key_type a, const utl_key_type b) {
  if (!a || !b) {
    utl_error_func("One or both of the strings are null", utl_user_defined_data);
    return 0;
  }
  int result = strcmp((const char *)a, (const char *)b);
  return result;
}

static void string_deallocator_json(void *data) {
  if (!data) {
    return;
  }
  free(data);
}

static void serialize_string(const char *value, utl_string *str) {
  utl_string_append(str, "\"");
  utl_string_append(str, value);
  utl_string_append(str, "\"");
}

static void serialize_array(const utl_json_element *element, utl_string *str) {
  utl_string_append(str, "[");
  for (size_t i = 0; i < utl_vector_size(element->value.array_val); ++i) {
    utl_json_serialize_internal(*(utl_json_element **)utl_vector_at(element->value.array_val, i), str);
    if (i < utl_vector_size(element->value.array_val) - 1) {
      utl_string_append(str, ", ");
    }
  }
  utl_string_append(str, "]");
}

static void serialize_object(const utl_json_element *element, utl_string *str) {
  utl_string_append(str, "{");
  utl_map_iterator it = utl_map_begin(element->value.object_val);
  utl_map_iterator end = utl_map_end(element->value.object_val);
  bool first = true;
  while (it.node != end.node) {
    if (!first) {
      utl_string_append(str, ", ");
    }
    serialize_string((char *)it.node->key, str);
    utl_string_append(str, ": ");
    utl_json_serialize_internal((utl_json_element *)it.node->value, str);
    first = false;
    utl_map_iterator_increment(&it);
  }
  utl_string_append(str, "}");
}

static void utl_json_serialize_internal(const utl_json_element *element, utl_string *str) {
  if (!element) {
    utl_string_append(str, "null");
    return;
  }
  switch (element->type) {
    case UTL_JSON_OBJECT:
      serialize_object(element, str);
      break;
    case UTL_JSON_ARRAY:
      serialize_array(element, str);
      break;
    case UTL_JSON_STRING:
      serialize_string(element->value.string_val, str);
      break;
    case UTL_JSON_NUMBER: {
      char buffer[64];
      snprintf(buffer, sizeof(buffer), "%g", element->value.number_val);
      utl_string_append(str, buffer); }
      break;
    case UTL_JSON_BOOL:
      utl_string_append(str, element->value.bool_val ? "true" : "false");
      break;
    case UTL_JSON_NULL:
      utl_string_append(str, "null");
      break;
    default:
      utl_string_append(str, "unknown");
  }
}

static void append_indent(utl_string *str, int indent) {
  for (int i = 0; i < indent; ++i) {
    utl_string_append(str, " ");
  }
}

static void format_string(const char *value, utl_string *str) {
  utl_string_append(str, "\"");
  utl_string_append(str, value);
  utl_string_append(str, "\"");
}

static void format_array(const utl_json_element *element, utl_string *str, int indent) {
  utl_string_append(str, "[\n");
  for (size_t i = 0; i < utl_vector_size(element->value.array_val); ++i) {
    append_indent(str, indent + 2);
    utl_json_format_internal(*(utl_json_element **)utl_vector_at(element->value.array_val, i), str, indent + 2);
    if (i < utl_vector_size(element->value.array_val) - 1) {
      utl_string_append(str, ",");
    }
    utl_string_append(str, "\n");
  }
  append_indent(str, indent);
  utl_string_append(str, "]");
}

static void format_object(const utl_json_element *element, utl_string *str, int indent) {
  utl_string_append(str, "{\n");
  utl_map_iterator it = utl_map_begin(element->value.object_val);
  utl_map_iterator end = utl_map_end(element->value.object_val);
  bool first = true;
  while (it.node != end.node) {
    if (!first) {
      utl_string_append(str, ",\n");
    }
    append_indent(str, indent + 2);
    format_string((char *)it.node->key, str);
    utl_string_append(str, ": ");
    utl_json_format_internal((utl_json_element *)it.node->value, str, indent + 2);
    first = false;
    utl_map_iterator_increment(&it);
  }
  if (!first) {
    utl_string_append(str, "\n");
  }
  append_indent(str, indent);
  utl_string_append(str, "}");
}

static void utl_json_format_internal(const utl_json_element *element, utl_string *str, int indent) {
  if (!element) {
    utl_string_append(str, "null");
    return;
  }
  switch (element->type) {
    case UTL_JSON_OBJECT:
      format_object(element, str, indent);
      break;
    case UTL_JSON_ARRAY:
      format_array(element, str, indent);
      break;
    case UTL_JSON_STRING:
      format_string(element->value.string_val, str);
      break;
    case UTL_JSON_NUMBER: {
      char buffer[64];
      snprintf(buffer, sizeof(buffer), "%g", element->value.number_val);
      utl_string_append(str, buffer); }
      break;
    case UTL_JSON_BOOL:
      utl_string_append(str, element->value.bool_val ? "true" : "false");
      break;
    case UTL_JSON_NULL:
      utl_string_append(str, "null");
      break;
    default:
      utl_error_func("Unknown JSON type", utl_user_defined_data);
      utl_string_append(str, "unknown");
  }
}

utl_json_element *utl_json_create(utl_json_type type) {
  utl_json_element *element = (utl_json_element *)malloc(sizeof(utl_json_element));
  if (!element) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  element->type = type;
  switch (type) {
    case UTL_JSON_NULL:
      break;
    case UTL_JSON_BOOL:
      element->value.bool_val = false;
      break;
    case UTL_JSON_NUMBER:
      element->value.number_val = 0.0;
      break;
    case UTL_JSON_STRING:
      element->value.string_val = NULL;
      break;
    case UTL_JSON_ARRAY:
      element->value.array_val = utl_vector_create(sizeof(utl_json_element *));
      if (!element->value.array_val) {
        utl_error_func("Memory allocation failed for JSON array", utl_user_defined_data);
        free(element);
        return NULL;
      }
      break;
    case UTL_JSON_OBJECT:
        element->value.object_val = utl_map_create(compare_strings_json, string_deallocator_json, json_element_deallocator);
        if (!element->value.object_val) {
          utl_error_func("Memory allocation failed for JSON object", utl_user_defined_data);
          free(element);
          return NULL;
        }
        break;
    default:
      utl_error_func("Invalid JSON type", utl_user_defined_data);
      free(element);
      return NULL;
  }
  return element;
}

utl_json_element *utl_json_parse(const char *json_str) {
  if (!json_str) { 
    utl_error_func("JSON string is null and invalid", utl_user_defined_data);
    return NULL;
  }
  utl_json_parser_state state;
  state.input = utl_string_strdup(json_str);
  if (!state.input) {
    utl_error_func("Memory allocation failed for duplicating JSON string", utl_user_defined_data);
    return NULL;
  }
  state.position = 0;
  state.input_len = utl_string_length_cstr(json_str);
  utl_next_token(&state);
  utl_json_element *root = NULL;
  if (state.current_token.type == UTL_JSON_TOKEN_OBJECT_START) {
    root = utl_parse_object(&state);
  } 
  else if (state.current_token.type == UTL_JSON_TOKEN_ARRAY_START) {
    root = utl_parse_array(&state);
  } 
  else {
    root = utl_parser_internal(&state);
    if (root == NULL) {
      utl_error_func("Invalid JSON value", utl_user_defined_data);
      free(state.input);
      return NULL;
    }
  }
  if (root == NULL) {
    utl_error_func("Can't parse JSON", utl_user_defined_data);
    free(state.input);
    return NULL;
  }
  utl_next_token(&state);
  if (state.current_token.type != UTL_JSON_TOKEN_EOF) {
    utl_error_func("Unexpected data after root element", utl_user_defined_data);
    utl_json_deallocate(root);
    free(state.input);
    return NULL;
  }
  free(state.input);
  return root;
}

utl_json_element *utl_json_read_from_file(const char *filename) {
  if (!filename) {
    utl_error_func("Filename is null and invalid", utl_user_defined_data);
    return NULL;
  }
  utl_file_reader *json = utl_file_reader_open(filename, UTL_READ_TEXT);
  if (!json) {
    utl_error_func("Failed to open file", utl_user_defined_data);
    return NULL;
  }
  size_t json_file_size = utl_file_reader_get_size(json);
  char *buffer = (char *)malloc(sizeof(char) * (json_file_size + 1));
  if (!buffer) {
    utl_error_func("Memory allocation failed for buffer", utl_user_defined_data);
    utl_file_reader_close(json);
    return NULL;
  }
  size_t size = utl_file_reader_read(buffer, sizeof(char), json_file_size, json);
  if (size == 0) {
    utl_error_func("Cannot read JSON file", utl_user_defined_data);
    free(buffer);
    utl_file_reader_close(json);
    return NULL;
  }
  buffer[size] = '\0';
  utl_json_element *json_element = utl_json_parse(buffer);
  if (!json_element) {
    utl_error_func("Failed to parse JSON data from file", utl_user_defined_data);
  }
  free(buffer);
  utl_file_reader_close(json);
  return json_element;
}

void utl_json_print(const utl_json_element *element) {
  if (!element) {
    utl_error_func("The provided JSON element is null", utl_user_defined_data);
    printf("null\n");
    return;
  }
  json_print_internal(element, 2);
  printf("\n");
}

utl_json_element *utl_json_get_element(const utl_json_element *element, const char *key_or_index) {
  if (!element || !key_or_index) {
    utl_error_func("Invalid arguments", utl_error_func);
    return NULL;
  }
  switch (element->type) {
    case UTL_JSON_OBJECT: {
      char *non_const_key = utl_string_strdup(key_or_index);
      if (!non_const_key) {
        utl_error_func("Memory allocation failed", utl_user_defined_data);
        return NULL;
      }
      utl_json_element *result = (utl_json_element *)utl_map_at(element->value.object_val, non_const_key);
      free(non_const_key);
      return result; }
    case UTL_JSON_ARRAY: {
      char *end;
      long index = strtol(key_or_index, &end, 10);
      if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= utl_vector_size(element->value.array_val)) {
        utl_error_func("Invalid index '%s'", utl_user_defined_data);
        return NULL;
      }
      return *(utl_json_element **)utl_vector_at(element->value.array_val, (size_t)index); }
    default:
      utl_error_func("Attempted to access non-object/non-array JSON element", utl_user_defined_data);
      return NULL;
  }
}

size_t utl_json_array_size(const utl_json_element *array) {
  if (!array) {
    utl_error_func("The provided JSON element is null", utl_user_defined_data);
    return 0;
  }
  if (array->type != UTL_JSON_ARRAY) {
    utl_error_func("The provided JSON element is not an array", utl_user_defined_data);
    return 0;
  }
  return utl_vector_size(array->value.array_val);
}

size_t utl_json_object_size(const utl_json_element *object) {
  if (!object) {
    utl_error_func("The provided JSON element is null", utl_user_defined_data);
    return 0;
  }
  if (object->type != UTL_JSON_OBJECT) {
    utl_error_func("The provided JSON element is not an object", utl_user_defined_data);
    return 0;
  }
  return utl_map_size(object->value.object_val);
}

utl_json_element *utl_json_deep_copy(const utl_json_element *element) {
  if (!element) {
    utl_error_func("The provided JsonElement is null", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *copy = utl_json_create(element->type);
  if (!copy) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  utl_map_iterator it, end;
  switch (element->type) {
    case UTL_JSON_NULL:
      break;
    case UTL_JSON_BOOL:
      copy->value.bool_val = element->value.bool_val;
      break;
    case UTL_JSON_NUMBER:
      copy->value.number_val = element->value.number_val;
      break;
    case UTL_JSON_STRING:
      copy->value.string_val = utl_string_strdup(element->value.string_val);
      if (!copy->value.string_val) {
        utl_error_func("Memory allocation failed for string", utl_user_defined_data);
        utl_json_deallocate(copy);
        return NULL;
      }
      break;
    case UTL_JSON_ARRAY:
      for (size_t i = 0; i < utl_vector_size(element->value.array_val); ++i) {
        utl_json_element *item = utl_json_deep_copy(*(utl_json_element **)utl_vector_at(element->value.array_val, i));
        if (!item) {
          utl_json_deallocate(copy);
          utl_error_func("Memory allocation failed while copying array element", utl_user_defined_data);
          return NULL;
        }
        if (!utl_vector_push_back(copy->value.array_val, &item)) {
          utl_json_deallocate(item);
          utl_json_deallocate(copy);
          utl_error_func("Failed to push array element into new copy", utl_user_defined_data);
          return NULL;
        }
      }
      break;
    case UTL_JSON_OBJECT:
      it = utl_map_begin(element->value.object_val);
      end = utl_map_end(element->value.object_val);
      while (it.node != end.node) {
        char *key = utl_string_strdup((char *)it.node->key);
        if (!key) {
          utl_error_func("Memory allocation failed for object key", utl_user_defined_data);
          utl_json_deallocate(copy);
          return NULL;
        }
        utl_json_element *val_copy = utl_json_deep_copy((utl_json_element *)it.node->value);
        if (!val_copy) {
          free(key);
          utl_json_deallocate(copy);
          utl_error_func("Memory allocation failed for object value", utl_user_defined_data);
          return NULL;
        }
        utl_map_insert(copy->value.object_val, key, val_copy);
        utl_map_iterator_increment(&it);
      }
      break;
    default:
      utl_error_func("Unknown JSON type", utl_user_defined_data);
      utl_json_deallocate(copy);
      return NULL;
  }
  return copy;
}

utl_json_type utl_json_type_of_element(const utl_json_element *element) {
  if (!element) {
    utl_error_func("The provided JSON element is null", utl_user_defined_data);
    return UTL_JSON_NULL;
  }
  return element->type;
}

bool utl_json_write_to_file(const utl_json_element *element, const char *filename) {
  if (!element || !filename) {
    utl_error_func("Invalid arguments", utl_user_defined_data);
    return false;
  }
  char *jsonString = utl_json_serialize(element);
  if (!jsonString) {
    utl_error_func("Failed to serialize JSON element", utl_user_defined_data);
    return false;
  }
  utl_file_writer *writer = utl_file_writer_open(filename, UTL_WRITE_TEXT);
  if (!writer || !utl_file_writer_is_open(writer)) {
    utl_error_func("Failed to open file for writing", utl_user_defined_data);
    free(jsonString);
    return false;
  }
  size_t written = utl_file_writer_write(jsonString, sizeof(char), strlen(jsonString), writer);
  free(jsonString);
  if (written == 0) {
    utl_error_func("Failed to write to file", utl_user_defined_data);
    utl_file_writer_close(writer);
    return false;
  }
  utl_file_writer_close(writer);
  return true;
}

char *utl_json_serialize(const utl_json_element *element) {
  if (!element) {
    utl_error_func("The provided JSON element is null", utl_user_defined_data);
    return NULL;
  }
  utl_string *str = utl_string_create("");
  if (!str) {
    utl_error_func("Memory allocation failed for string creation", utl_user_defined_data);
    return NULL;
  }
  utl_json_serialize_internal(element, str);
  const char *temp = utl_string_c_str(str);
  char *serialized = utl_string_strdup(temp);
  if (!serialized) {
    utl_error_func("Memory allocation failed for string duplication", utl_user_defined_data);
    utl_string_deallocate(str);
    return NULL;
  }
  utl_string_deallocate(str); 
  return serialized; 
}

bool utl_json_compare(const utl_json_element *element1, const utl_json_element *element2) {
  if (element1 == element2) {
    return true;
  }
  if (element1 == NULL || element2 == NULL) {
    return false;
  }
  if (element1->type != element2->type) {
    return false;
  }
  switch (element1->type) {
    case UTL_JSON_NULL:
      return true;
    case UTL_JSON_BOOL:
      return element1->value.bool_val == element2->value.bool_val;
    case UTL_JSON_NUMBER:
      return element1->value.number_val == element2->value.number_val;
    case UTL_JSON_STRING:
      return strcmp(element1->value.string_val, element2->value.string_val) == 0;
    case UTL_JSON_ARRAY:
      if (utl_vector_size(element1->value.array_val) != utl_vector_size(element2->value.array_val)) {
        return false;
      }
      for (size_t i = 0; i < utl_vector_size(element1->value.array_val); ++i) {
        utl_json_element *item1 = *(utl_json_element **)utl_vector_at(element1->value.array_val, i);
        utl_json_element *item2 = *(utl_json_element **)utl_vector_at(element2->value.array_val, i);
        if (!utl_json_compare(item1, item2)) {
          return false;
        }
      }
      return true;
    case UTL_JSON_OBJECT:
      if (utl_map_size(element1->value.object_val) != utl_map_size(element2->value.object_val)) {
        return false;
      }
      utl_map_iterator it1 = utl_map_begin(element1->value.object_val);
      utl_map_iterator end1 = utl_map_end(element1->value.object_val);
      while (it1.node != end1.node) {
        const char *key = (char *)it1.node->key;
        utl_json_element *val1 = (utl_json_element *)it1.node->value;
        utl_json_element *val2 = (utl_json_element *)utl_map_at(element2->value.object_val, (utl_key_type)key);
        if (!val2 || !utl_json_compare(val1, val2)) {
          return false;
        }
        utl_map_iterator_increment(&it1);
      }
      return true;
    default:
      utl_error_func("Unknown JSON type", utl_user_defined_data);
      return false;
  }
}

bool utl_json_set_element(utl_json_element *element, const char *key_or_index, utl_json_element *new_element) {
  if (!element || !key_or_index || !new_element) {
    utl_error_func("Invalid arguments. JSON Element, key/index, or new element is null", utl_user_defined_data);
    return false;
  }
  switch (element->type) {
    case UTL_JSON_OBJECT: {
      char *non_const_key = utl_string_strdup(key_or_index);
      if (!non_const_key) {
        utl_error_func("Memory allocation failed for key duplication", utl_user_defined_data);
        return false;
      }
      utl_map_insert(element->value.object_val, non_const_key, new_element);
      return true; }
    case UTL_JSON_ARRAY: {
      char *end;
      long index = strtol(key_or_index, &end, 10);
      if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= utl_vector_size(element->value.array_val)) {
        utl_error_func("Invalid index", utl_user_defined_data);
        return false;
      }
      utl_json_element **array_element = utl_vector_at(element->value.array_val, (size_t)index);
      utl_json_deallocate(*array_element);
      *array_element = new_element;
      return true; }
    default:
      utl_error_func("JSON Element is not an object or an array", utl_user_defined_data);
      return false;
  }
}

bool utl_json_remove_element(utl_json_element *element, const char *key_or_index) {
  if (!element || !key_or_index) {
    utl_error_func("Invalid arguments. JSON Element or key/index is null", utl_user_defined_data);
    return false;
  }
  switch (element->type) {
    case UTL_JSON_OBJECT: {
      char *non_const_key = utl_string_strdup(key_or_index);
      if (!non_const_key) {
        utl_error_func("Memory allocation failed for key duplication", utl_user_defined_data);
        return false;
      }
      bool result = utl_map_erase(element->value.object_val, non_const_key);
      free(non_const_key);
      return result; }
    case UTL_JSON_ARRAY: {
      char *end;
      long index = strtol(key_or_index, &end, 10);
      if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= utl_vector_size(element->value.array_val)) {
        utl_error_func("Invalid array index", utl_user_defined_data);
        return false;
      }
      utl_json_element **array_element = utl_vector_at(element->value.array_val, (size_t)index);
      utl_json_deallocate(*array_element);
      utl_vector_erase(element->value.array_val, (size_t)index, 1);
      return true; }
    default:
      utl_error_func("JSON Element is neither an object nor an array", utl_user_defined_data);
      return false;
  }
}

utl_json_element *utl_json_find(const utl_json_element *element, utl_json_predicate predicate, void *user_data) {
  if (!element || !predicate) {
    utl_error_func("Invalid argument. JSON Element or predicate is null", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *found_element = NULL;
  switch (element->type) {
    case UTL_JSON_OBJECT:
      if (utl_json_find_in_object(element, predicate, user_data, &found_element)) {
        return found_element;
      }
      break;
    case UTL_JSON_ARRAY:
      if (utl_json_find_in_array(element, predicate, user_data, &found_element)) {
        return found_element;
      }
      break;
    default:
      utl_error_func("JSON Element is not an object or array", utl_user_defined_data);
      return NULL;
  }
  return NULL;
}

utl_json_element *utl_json_merge(const utl_json_element *element1, const utl_json_element *element2) {
  if (!element1 || !element2 || element1->type != UTL_JSON_OBJECT || element2->type != UTL_JSON_OBJECT) {
    utl_error_func("Both elements must be JSON objects", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *merged = utl_json_create(UTL_JSON_OBJECT);
  if (!merged) {
    utl_error_func("Memory allocation failed for the merged object", utl_user_defined_data);
    return NULL;
  }
  utl_map_iterator it = utl_map_begin(element1->value.object_val);
  utl_map_iterator end = utl_map_end(element1->value.object_val);
  while (it.node != end.node) {
    char *key = utl_string_strdup((char *)it.node->key);
    if (!key) {
      utl_error_func("Memory allocation failed for key in the first object", utl_user_defined_data);
      utl_json_deallocate(merged);
      return NULL;
    }
    utl_json_element *value_copy = utl_json_deep_copy((utl_json_element *)it.node->value);
    if (!value_copy) {
      utl_error_func("Memory allocation failed while copying value from the first object", utl_user_defined_data);
      free(key);
      utl_json_deallocate(merged);
      return NULL;
    }
    utl_map_insert(merged->value.object_val, key, value_copy);
    utl_map_iterator_increment(&it);
  }
  it = utl_map_begin(element2->value.object_val);
  end = utl_map_end(element2->value.object_val);
  while (it.node != end.node) {
    char *key = utl_string_strdup((char *)it.node->key);
    if (!key) {
      utl_error_func("Memory allocation failed for key in the second object", utl_user_defined_data);
      utl_json_deallocate(merged);
      return NULL;
    }
    utl_json_element *value_copy = utl_json_deep_copy((utl_json_element *)it.node->value);
    if (!value_copy) {
      utl_error_func("Memory allocation failed while copying value from the second object", utl_user_defined_data);
      free(key);
      utl_json_deallocate(merged);
      return NULL;
    }
    utl_map_insert(merged->value.object_val, key, value_copy);
    utl_map_iterator_increment(&it);
  }
  return merged;
}

char **utl_json_to_string_array(const utl_json_element *array, size_t *length) {
  if (!array || array->type != UTL_JSON_ARRAY || !length) {
    utl_error_func("Invalid input", utl_user_defined_data);
    return NULL;
  }
  *length = utl_vector_size(array->value.array_val); 
  if (*length == 0) {
    utl_error_func("Size of string array element in JSON is zero", utl_user_defined_data);
    return NULL;
  }
  char **string_array = malloc(*length * sizeof(char *));
  if (!string_array) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    *length = 0;
    return NULL;
  }
  for (size_t i = 0; i < *length; ++i) {
    utl_json_element *element = *(utl_json_element **)utl_vector_at(array->value.array_val, i);
    if (!element || element->type != UTL_JSON_STRING) {
      utl_error_func("Non-string element found", utl_user_defined_data);
      while (i > 0) {
        free(string_array[--i]);
      }
      free(string_array);
      *length = 0;
      return NULL;
    }
    string_array[i] = utl_string_strdup(element->value.string_val);
  }
  return string_array;
}

void *utl_json_convert(const utl_json_element *element, utl_json_type type) {
  if (!element) {
    utl_error_func("The provided JSON element is null", utl_user_defined_data);
    return NULL;
  }
  switch (type) {
    case UTL_JSON_STRING:
      if (element->type == UTL_JSON_NUMBER) {
        utl_json_element *stringElement = utl_json_create(UTL_JSON_STRING);
        if (!stringElement) {
          utl_error_func("Memory allocation failed for JSON string element", utl_user_defined_data);
          return NULL;
        }
        char *str = (char *)malloc(64 * sizeof(char));
        if (!str) {
          utl_error_func("Memory allocation failed for number to string buffer", utl_user_defined_data);
          utl_json_deallocate(stringElement);  
          return NULL;
        }
        snprintf(str, 64, "%g", element->value.number_val);
        stringElement->value.string_val = str;
        return stringElement;
      } 
      else if (element->type == UTL_JSON_BOOL) {
        utl_json_element *stringElement = utl_json_create(UTL_JSON_STRING);
        if (!stringElement) {
          utl_error_func("Memory allocation failed for JSON bool to JSON string conversion", utl_user_defined_data);
          return NULL;
        }
        if (!stringElement->value.string_val) {
          utl_error_func("Memory allocation failed for string duplication", utl_user_defined_data);
          utl_json_deallocate(stringElement);
          return NULL;
        }
        stringElement->value.string_val = utl_string_strdup(element->value.bool_val ? "true" : "false");
        return stringElement;
      }
      utl_error_func("Conversion to string is not supported for the given type", utl_user_defined_data);
      return NULL;
    case UTL_JSON_NUMBER:
      if (element->type == UTL_JSON_STRING) {
        double *num = (double *)malloc(sizeof(double));
        if (!num) {
          utl_error_func("Memory allocation failed for string to number conversion", utl_user_defined_data);  
          return NULL;
        }
        *num = strtod(element->value.string_val, NULL);
        return num;
      }
      utl_error_func("Conversion to number is not supported for the given type", utl_user_defined_data);
      return NULL;
    case UTL_JSON_BOOL:
      if (element->type == UTL_JSON_NUMBER) {
        bool *bool_val = (bool *)malloc(sizeof(bool));
        if (!bool_val) {
          utl_error_func("Memory allocation failed for number to boolean conversion", utl_user_defined_data);  
          return NULL;
        }
        *bool_val = (element->value.number_val != 0);
        return bool_val;
      }
      utl_error_func("Conversion to boolean is not supported for the given type", utl_user_defined_data); 
      return NULL;
    case UTL_JSON_ARRAY: {
      utl_json_element *array = utl_json_create(UTL_JSON_ARRAY);
      if (!array) {
        utl_error_func("Memory allocation failed for array", utl_user_defined_data);  
        return NULL;
      }
      switch (element->type) {
        case UTL_JSON_STRING: {
          size_t i = 0;
          size_t len = strlen(element->value.string_val);
          while (i < len) {
            size_t char_len = utl_string_utf8_char_len(element->value.string_val[i]);
            if (char_len == 0 || i + char_len > len) {
              utl_json_deallocate(array);
              utl_error_func("Invalid UTF-8", utl_user_defined_data); 
              return NULL;
            }
            char *utf8_char = (char *)malloc((char_len + 1) * sizeof(char));
            if (!utf8_char) {
              utl_json_deallocate(array);
              utl_error_func("Memory allocation failed for UTF-8 character", utl_user_defined_data);  
              return NULL;
            }
            strncpy(utf8_char, element->value.string_val + i, char_len);
            utf8_char[char_len] = '\0';
            utl_json_element *charElement = utl_json_create(UTL_JSON_STRING);
            charElement->value.string_val = utf8_char;
            utl_vector_push_back(array->value.array_val, &charElement);
            i += char_len;
          }
          break; }
        case UTL_JSON_NUMBER: {
          utl_json_element *numElement = utl_json_create(UTL_JSON_NUMBER);
          numElement->value.number_val = element->value.number_val;
          utl_vector_push_back(array->value.array_val, &numElement);
          break; }
        case UTL_JSON_BOOL: {
          utl_json_element *boolElement = utl_json_create(UTL_JSON_BOOL);
          boolElement->value.bool_val = element->value.bool_val;
          utl_vector_push_back(array->value.array_val, &boolElement);
          break; }
        case UTL_JSON_NULL: {
          utl_json_element *nullElement = utl_json_create(UTL_JSON_NULL);
          utl_vector_push_back(array->value.array_val, &nullElement);
          break; }
        case UTL_JSON_OBJECT: {
          utl_json_element *objElement = utl_json_deep_copy(element); 
          utl_vector_push_back(array->value.array_val, &objElement);
          break; }
        case UTL_JSON_ARRAY:
        default: {
          utl_json_element *arrElement = utl_json_deep_copy(element); 
          utl_vector_push_back(array->value.array_val, &arrElement);
          break; }
      }
      return array; }
      break;
    case UTL_JSON_OBJECT: {
      utl_json_element *object = utl_json_create(UTL_JSON_OBJECT);
      if (!object) {
        utl_error_func("Memory allocation failed for object", utl_user_defined_data);  
        return NULL;
      }
      char *key = utl_string_strdup("value");
      utl_json_element *newElement = NULL;
      switch (element->type) {
        case UTL_JSON_NUMBER:
          newElement = utl_json_create(UTL_JSON_NUMBER);
          newElement->value.number_val = element->value.number_val;
          break;
        case UTL_JSON_STRING:
          newElement = utl_json_create(UTL_JSON_STRING);
          newElement->value.string_val = utl_string_strdup(element->value.string_val);
          break;
        case UTL_JSON_BOOL:
          newElement = utl_json_create(UTL_JSON_BOOL);
          newElement->value.bool_val = element->value.bool_val;
          break;
        case UTL_JSON_NULL:
          newElement = utl_json_create(UTL_JSON_NULL);
          break;
        default:
          utl_error_func("Unsupported JSON element type for object conversion", utl_user_defined_data);  
          utl_json_deallocate(object);
          free(key); 
          return NULL;
      }
      if (!newElement) {
        utl_error_func("Failed to create new JSON element", utl_user_defined_data);  
        utl_json_deallocate(object);
        free(key); 
        return NULL;
      }
      utl_map_insert(object->value.object_val, key, newElement);
      return object; }
      break;
    default:
      utl_error_func("Unsupported conversion type", utl_user_defined_data);  
      return NULL;
  }
  return NULL;
}

utl_json_element *utl_json_map(const utl_json_element *array, utl_json_map_function map_func, void *user_data) {
  if (!array || array->type != UTL_JSON_ARRAY) {
    utl_error_func("Invalid input array", utl_user_defined_data);
    return NULL;
  }
  if (!map_func) {
    utl_error_func("Map function is null", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *resultArray = utl_json_create(UTL_JSON_ARRAY);
  if (!resultArray) {
    utl_error_func("Memory allocation failed for result array", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < utl_vector_size(array->value.array_val); ++i) {
    utl_json_element *currentElement = *(utl_json_element **)utl_vector_at(array->value.array_val, i);
    utl_json_element *transformedElement = map_func(currentElement, user_data);
    if (!transformedElement) {
      utl_error_func("Transformation failed for JSON element", utl_user_defined_data);
      for (size_t j = 0; j < i; ++j) {
        utl_json_element *previousElement = *(utl_json_element **)utl_vector_at(resultArray->value.array_val, j);
        utl_json_deallocate(previousElement);
      }
      utl_vector_deallocate(resultArray->value.array_val);
      free(resultArray);
      return NULL;
    }
    utl_vector_push_back(resultArray->value.array_val, &transformedElement);
  }
  return resultArray;
}

utl_json_element *utl_json_filter(const utl_json_element *array, utl_json_predicate predicate, void *user_data) {
  if (!array || array->type != UTL_JSON_ARRAY) {
    utl_error_func("Invalid array input", utl_user_defined_data);
    return NULL;
  }
  if (!predicate) {
    utl_error_func("Predicate function is null", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *resultArray = utl_json_create(UTL_JSON_ARRAY);
  if (!resultArray) {
    utl_error_func("Memory allocation failed for the result array", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < utl_vector_size(array->value.array_val); ++i) {
    utl_json_element *currentElement = *(utl_json_element **)utl_vector_at(array->value.array_val, i);
    if (predicate(currentElement, user_data)) {
      utl_json_element *elementCopy = utl_json_deep_copy(currentElement);
      if (!elementCopy) {
        utl_error_func("Memory allocation failed during deep copy", utl_user_defined_data);
        for (size_t j = 0; j < utl_vector_size(resultArray->value.array_val); ++j) {
          utl_json_element *previousElement = *(utl_json_element **)utl_vector_at(resultArray->value.array_val, j);
          utl_json_deallocate(previousElement);
        }
        utl_vector_deallocate(resultArray->value.array_val);
        free(resultArray);
        return NULL;
      }
      utl_vector_push_back(resultArray->value.array_val, &elementCopy);
    }
  }
  return resultArray;
}

void *utl_json_reduce(const utl_json_element *array, utl_json_reduce_function reduce_func, void *initial_value, void *user_data) {
  if (!array || array->type != UTL_JSON_ARRAY) {
    utl_error_func("Invalid array input", utl_user_defined_data);
    return NULL;
  }
  if (!reduce_func) {
    utl_error_func("Reduce function is null", utl_user_defined_data);
    return NULL;
  }
  void *accumulator = initial_value;
  for (size_t i = 0; i < utl_vector_size(array->value.array_val); ++i) {
    utl_json_element *currentElement = *(utl_json_element **)utl_vector_at(array->value.array_val, i);
    accumulator = reduce_func(currentElement, accumulator, user_data);
  }
  return accumulator;
}

char *utl_json_format(const utl_json_element *element) {
  if (!element) {
    utl_error_func("The provided JSON element is null", utl_user_defined_data);
    return NULL;
  }
  utl_string *str = utl_string_create("");
  utl_json_format_internal(element, str, 0);
  const char *temp = utl_string_c_str(str);
  char *formatted = utl_string_strdup(temp);
  if (!formatted) {
    utl_error_func("Memory allocation failed for formatted string", utl_user_defined_data);
    utl_string_deallocate(str);
    return NULL;
  }
  utl_string_deallocate(str);
  return formatted;
}

utl_json_element *utl_json_clone(const utl_json_element *element) {
  if (!element) {
    utl_error_func("Null input provided", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *clonedElement = (utl_json_element *)malloc(sizeof(utl_json_element));
  if (!clonedElement) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  clonedElement->type = element->type;
  switch (element->type) {
    case UTL_JSON_NULL:
    case UTL_JSON_BOOL:
    case UTL_JSON_NUMBER:
      clonedElement->value = element->value;
      break;
    case UTL_JSON_STRING:
      clonedElement->value.string_val = utl_string_strdup(element->value.string_val);
      if (!clonedElement->value.string_val) {
        utl_error_func("String duplication failed", utl_user_defined_data);
        free(clonedElement);
        return NULL;
      }
      break;
    case UTL_JSON_ARRAY:
    case UTL_JSON_OBJECT:
      clonedElement->value = element->value;
      break;
    default:
      utl_error_func("Unknown type encountered", utl_user_defined_data);
      free(clonedElement);
      return NULL;
    }
  return clonedElement;
}

char **utl_json_get_keys(const utl_json_element *object, size_t *num_keys) {
  if (!object || object->type != UTL_JSON_OBJECT || !num_keys) {
    utl_error_func("Invalid input", utl_user_defined_data);
    return NULL;
  }
  *num_keys = utl_map_size(object->value.object_val);
  char **keys = (char **)malloc(*num_keys * sizeof(char*));
  if (!keys) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  size_t i = 0;
  utl_map_iterator it = utl_map_begin(object->value.object_val);
  utl_map_iterator end = utl_map_end(object->value.object_val);
  while (it.node != end.node) {
    keys[i] = utl_string_strdup((char *)it.node->key);
    if (!keys[i]) {
      utl_error_func("Memory allocation failed for key duplication", utl_user_defined_data);
      while (i > 0) {
        free(keys[--i]);
      }
      free(keys);
      return NULL;
    }
    i++;
    utl_map_iterator_increment(&it);
  }
  return keys;
}

bool utl_json_add_to_array(utl_json_element *element1, utl_json_element *element2) {
  if (!element1) {
    utl_error_func("Element 1 is null", utl_user_defined_data);
    return false;
  }
  if (!element2) {
    utl_error_func("Element 2 is null", utl_user_defined_data);
    return false;
  }
  if (utl_vector_push_back(element1->value.array_val, &element2)) {
    return true;
  }
  utl_error_func("Failed to add element to array", utl_user_defined_data);
  return false;
}

bool utl_json_add_to_object(utl_json_element *object, const char *key, utl_json_element *value) {
  if (!object || object->type != UTL_JSON_OBJECT) {
    utl_error_func("Target is not a JSON object", utl_user_defined_data);
    return false;
  }
  if (!key) {
    utl_error_func("Key is null", utl_user_defined_data);
    return false;
  }
  if (!value) {
    utl_error_func("Value is null for key", utl_user_defined_data);
    return false;
  }
  char *duplicatedKey = utl_string_strdup(key);
  if (!duplicatedKey) {
    utl_error_func("Failed to duplicate key", utl_user_defined_data);
    return false;
  }
  utl_json_element *existingValue = (utl_json_element *)utl_map_at(object->value.object_val, duplicatedKey);
  if (existingValue) {
    utl_json_deallocate(existingValue);
  }
  bool insertResult = utl_map_insert(object->value.object_val, duplicatedKey, value);
  if (!insertResult) {
    free(duplicatedKey); 
    if (!existingValue) {
      utl_json_deallocate(value);
    }
    utl_error_func("Failed to insert or replace key-value pair for key", utl_user_defined_data);
    return false;
  }
  return true;
}

utl_json_element *utl_json_query(const utl_json_element *element, const char *query) {
  if (!element || !query) {
    utl_error_func("Invalid arguments", utl_user_defined_data);
    return NULL;
  }
  int count;
  char **tokens = split_query(query, &count);
  if (!tokens) {
    utl_error_func("Failed to split query", utl_user_defined_data);
    return NULL;
  }
  utl_json_element *currentElement = (utl_json_element *)element;
  for (int i = 0; i < count; ++i) {
    char *token = tokens[i];
    char *bracketPos = strchr(token, '[');
    if (bracketPos) {
      *bracketPos = '\0';
      char *indexStr = bracketPos + 1;
      char *endBracket = strchr(indexStr, ']');
      if (!endBracket) {
        utl_error_func("Invalid query format, missing closing bracket", utl_user_defined_data);
        break;
      }
      *endBracket = '\0';
      currentElement = utl_json_get_element(currentElement, token);
      if (!currentElement) {
        utl_error_func("JSON Element not found", utl_user_defined_data);
        break;
      }
      if (utl_json_type_of_element(currentElement) == UTL_JSON_ARRAY) {
        currentElement = utl_json_get_element(currentElement, indexStr);
        if (!currentElement) {
          utl_error_func("JSON Element not found", utl_user_defined_data);
          break;
        }
      } 
      else {
        utl_error_func("Expected an array but found a different type", utl_user_defined_data);
        currentElement = NULL;
        break;
      }
    } 
    else {
      currentElement = utl_json_get_element(currentElement, token);
      if (!currentElement) {
        utl_error_func("JSON Element not found", utl_user_defined_data);
        break;
      }
    }
  }
  for (int i = 0; i < count; ++i) {
    free(tokens[i]);
  }
  return currentElement;
}

