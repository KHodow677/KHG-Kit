#include "khg_utl/config.h"
#include "khg_utl/error_func.h"
#include "khg_utl/file_reader.h"
#include "khg_utl/file_writer.h"
#include "khg_utl/string.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static char *utl_trim_whitespace(char *str) {
  if (!str) {
    utl_error_func("Null string provided to trim whitespace", utl_user_defined_data);
    return NULL;
  }
  char *end;
  while (isspace((unsigned char)*str)) {  
    str++;
  }
  if (*str == 0) {  
    return str;
  }
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end)) {
    end--;
  }
  end[1] = '\0';
  return str;
}

static void utl_xor_encrypt_decrypt(const char *input, char *output, char key, size_t size) {
  if (!input || !output) {
    utl_error_func("Null input or output provided to xor_encrypt_decrypt", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < size; ++i) {
    output[i] = input[i] ^ key;
  }
}

utl_config_file *utl_config_create(const char *filename) {
  utl_file_reader *fr = utl_file_reader_open(filename, UTL_READ_TEXT);
  if (!fr) {
    utl_error_func("Unable to open file for reading", utl_user_defined_data);
    exit(-1);
  }
  utl_config_file *config = malloc(sizeof(utl_config_file));
  if (!config) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    utl_file_reader_close(fr);
    exit(-1);
  }
  config->sections = NULL;
  config->section_count = 0;
  config->default_section = NULL;
  config->filename = utl_string_strdup(filename);
  char line[1024];
  utl_config_section *current_section = NULL;
  while (utl_file_reader_read_line(line, sizeof(line), fr)) {
    utl_string *str = utl_string_create(line);
    utl_string_trim(str);
    const char *trimmed = utl_string_c_str(str);
    char *writable_trimmed = utl_string_strdup(trimmed);
    utl_string_deallocate(str);
    utl_config_entry entry = {0};
    entry.is_comment = false;
    entry.key = NULL;
    entry.value = NULL;
    if (writable_trimmed[0] == '#' || writable_trimmed[0] == ';') {
      entry.is_comment = true;
      entry.value = utl_string_strdup(writable_trimmed);
    }
    else if (writable_trimmed[0] == '[') {
      current_section = malloc(sizeof(utl_config_section));
      if (!current_section) {
        utl_error_func("Memory allocation failed", utl_user_defined_data);
        free(writable_trimmed);
        utl_file_reader_close(fr);
        exit(-1);
      }
      size_t section_name_length = strlen(writable_trimmed) - 2;
      current_section->section_name = malloc(section_name_length + 1);
      strncpy(current_section->section_name, writable_trimmed + 1, section_name_length);
      current_section->section_name[section_name_length] = '\0';
      current_section->entries = NULL;
      current_section->entry_count = 0;
      config->sections = realloc(config->sections, (config->section_count + 1) * sizeof(utl_config_section *));
      config->sections[config->section_count++] = current_section;
    } 
    else if (current_section && strchr(writable_trimmed, '=')) {
      char *key = strtok(writable_trimmed, "=");
      char *value = strtok(NULL, "");
      entry.key = utl_string_strdup(utl_trim_whitespace(key));
      entry.value = utl_string_strdup(utl_trim_whitespace(value));
    }
    if (current_section && (entry.is_comment || entry.key)) {
      current_section->entries = realloc(current_section->entries, (current_section->entry_count + 1) * sizeof(utl_config_entry));
      current_section->entries[current_section->entry_count++] = entry;
    }
    free(writable_trimmed);
  }
  utl_file_reader_close(fr);
  return config;
}

void utl_config_save(const utl_config_file *config, const char *filename) {
  utl_file_writer* fw = utl_file_writer_open(filename, UTL_WRITE_TEXT);
  if (!fw) {
    utl_error_func("Unable to open file for writing", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < config->section_count; i++) {
    utl_config_section *section = config->sections[i];
    utl_file_writer_write_fmt(fw, "[%s]\n", section->section_name);
    for (size_t j = 0; j < section->entry_count; j++) {
      utl_config_entry *entry = &section->entries[j];
      if (entry->is_comment) {
        utl_file_writer_write_fmt(fw, "%s\n", entry->value);
      }
      else if (entry->key && entry->value) {
        utl_file_writer_write_fmt(fw, "%s=%s\n", entry->key, entry->value);
      }
    }
    utl_file_writer_write_fmt(fw, "\n");
  }
  utl_file_writer_close(fw);
}

const char *utl_config_get_value(const utl_config_file *config, const char *section, const char *key) {
  if (!config) {
    utl_error_func("Config file pointer is null", utl_user_defined_data);
    return NULL;
  }
  if (!section) {
    utl_error_func("Section name is null", utl_user_defined_data);
    return NULL;
  }
  if (!key) {
    utl_error_func("Key name is null", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < config->section_count; ++i) {
    if (strcmp(config->sections[i]->section_name, section) == 0) {
      utl_config_section *sec = config->sections[i];
      for (size_t j = 0; j < sec->entry_count; ++j) {
        if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) {
          return sec->entries[j].value;
        }
      }
      break;
    }
  }
  return NULL;
}

void utl_config_set_value(utl_config_file *config, const char *section, const char *key, const char *value) {
  if (!config) {
    utl_error_func("Config file pointer is null", utl_user_defined_data);
    return;
  }
  if (!section) {
    utl_error_func("Section name is null", utl_user_defined_data);
    return;
  }
  if (!key) {
    utl_error_func("Key name is null", utl_user_defined_data);
    return;
  }
  if (!value) {
    utl_error_func("Value is null", utl_user_defined_data);
    return;
  }
  utl_config_section *sec = NULL;
  for (size_t i = 0; i < config->section_count; ++i) {
    if (strcmp(config->sections[i]->section_name, section) == 0) {
      sec = config->sections[i];
      break;
    }
  }
  if (!sec) {
    sec = malloc(sizeof(utl_config_section));
    if (!sec) {
      utl_error_func("Memory allocation failed for new section", utl_user_defined_data);
      return;
    }
    sec->section_name = utl_string_strdup(section);
    sec->entries = NULL;
    sec->entry_count = 0;
    config->sections = realloc(config->sections, (config->section_count + 1) * sizeof(utl_config_section *));
    if (!config->sections) {
      free(sec->section_name);
      free(sec);
      utl_error_func("Memory allocation failed for sections array", utl_user_defined_data);
      return;
    }
    config->sections[config->section_count++] = sec;
  }
  for (size_t j = 0; j < sec->entry_count; ++j) {
    if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) {
      free(sec->entries[j].value);
      sec->entries[j].value = utl_string_strdup(value);
      return;
    }
  }
  sec->entries = realloc(sec->entries, (sec->entry_count + 1) * sizeof(utl_config_entry));
  if (!sec->entries) {
    utl_error_func("Memory allocation failed for new entry", utl_user_defined_data);
    return;
  }
  sec->entries[sec->entry_count].key = utl_string_strdup(key);
  sec->entries[sec->entry_count].value = utl_string_strdup(value);
  sec->entry_count++;
}

void utl_config_remove_section(utl_config_file *config, const char *section) {
  if (!config || !section) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < config->section_count; ++i) {
    if (strcmp(config->sections[i]->section_name, section) == 0) {
      free(config->sections[i]->section_name);
      for (size_t j = 0; j < config->sections[i]->entry_count; ++j) {
        free(config->sections[i]->entries[j].key);
        free(config->sections[i]->entries[j].value);
      }
      free(config->sections[i]->entries);
      free(config->sections[i]);
      for (size_t k = i; k < config->section_count - 1; ++k) {
        config->sections[k] = config->sections[k + 1];
      }
      config->section_count--;
      config->sections = realloc(config->sections, config->section_count * sizeof(utl_config_section *));
      return;
    }
  }
}

void utl_config_remove_key(utl_config_file *config, const char *section, const char *key) {
  if (!config || !section || !key) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < config->section_count; ++i) {
    if (strcmp(config->sections[i]->section_name, section) == 0) {
      utl_config_section *sec = config->sections[i];
      for (size_t j = 0; j < sec->entry_count; ++j) {
        if (strcmp(sec->entries[j].key, key) == 0) {
          free(sec->entries[j].key);
          free(sec->entries[j].value);
          for (size_t k = j; k < sec->entry_count - 1; ++k) {
            sec->entries[k] = sec->entries[k + 1];
          }
          sec->entry_count--;
          sec->entries = realloc(sec->entries, sec->entry_count * sizeof(utl_config_entry));
          return;
        }
      }
      break;
    }
  }
}

void utl_config_deallocate(utl_config_file *config) {
  if (!config) {
    utl_error_func("Config file pointer is null", utl_error_func);
    return;
  }
  for (size_t i = 0; i < config->section_count; ++i) {
    free(config->sections[i]->section_name);
    for (size_t j = 0; j < config->sections[i]->entry_count; ++j) {
        free(config->sections[i]->entries[j].key);
        free(config->sections[i]->entries[j].value);
    }
    free(config->sections[i]->entries);
    free(config->sections[i]);
  }
  free(config->sections);
  free(config->default_section);
  free(config->filename);
  free(config);
}

bool utl_config_has_section(const utl_config_file *config, const char *section) {
  if (!config || !section) {
    utl_error_func("Invalid arguments provided.", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < config->section_count; ++i) {
    if (strcmp(config->sections[i]->section_name, section) == 0) {
      return true;
    }
  }
  return false;
}

bool utl_config_has_key(const utl_config_file *config, const char *section, const char *key) {
  if (!config || !section || !key) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < config->section_count; ++i) {
    if (strcmp(config->sections[i]->section_name, section) == 0) {
      utl_config_section *sec = config->sections[i];
      for (size_t j = 0; j < sec->entry_count; ++j) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
          utl_config_section *sec = config->sections[i];
          for (size_t j = 0; j < sec->entry_count; ++j) {   
            if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) { 
              return true;
            }
          }
          break;
        }
      }
      break;
    }
  }
  return false;
}

int utl_config_get_int(const utl_config_file *config, const char *section, const char *key, int default_value) {
  if (!config || !section || !key) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return default_value;
  }
  const char *value = utl_config_get_value(config, section, key);
  if (value) {
    char *end;
    long int_val = strtol(value, &end, 10);
    if (end != value && *end == '\0') {
      return (int)int_val; 
    }
  } 
  return default_value; 
}

double utl_config_get_double(const utl_config_file *config, const char *section, const char *key, double default_value) {
  if (!config || !section || !key) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return default_value;
  }
  const char *value = utl_config_get_value(config, section, key);
  if (value) {
    char *end;
    double double_val = strtod(value, &end);
    if (end != value && *end == '\0') {
      return double_val; 
    }
  } 
  return default_value;
}

bool utl_config_get_bool(const utl_config_file *config, const char *section, const char *key, bool default_value) {
  if (!config || !section || !key) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return default_value;
  }
  const char *value = utl_config_get_value(config, section, key);
  if (value) {
    if (strcasecmp(value, "true") == 0 || strcasecmp(value, "yes") == 0 || strcmp(value, "1") == 0) { 
      return true;
    }
    else if (strcasecmp(value, "false") == 0 || strcasecmp(value, "no") == 0 || strcmp(value, "0") == 0) { 
      return false;
    }
  }
  return default_value;
}

void utl_config_set_comment(utl_config_file *config, const char *section, const char *comment) {
  if (!config || !section || !comment) {
    utl_error_func("Invalid arguments provided", utl_error_func);
    return;
  }
  for (size_t i = 0; i < config->section_count; ++i) {
    utl_config_section *sec = config->sections[i];
    if (strcmp(config->sections[i]->section_name, section) == 0) {
      sec->comment = utl_string_strdup(comment); 
      return; 
    }
  }
}

utl_config_iterator utl_config_get_iterator(const utl_config_file *config) {
  utl_config_iterator iterator = {0};
  if (!config) {
    utl_error_func("Config file is null", utl_user_defined_data);
    return iterator; 
  }
  iterator.config = config;
  iterator.section_index = 0;
  iterator.entry_index = 0;
  return iterator;
}

bool utl_config_next_entry(utl_config_iterator *iterator, const char **section, const char **key, const char **value) {
  if (!iterator || !iterator->config || iterator->section_index >= iterator->config->section_count) {
    utl_error_func("Invalid iterator or configuration", utl_user_defined_data);
    return false;
  }
  while (iterator->section_index < iterator->config->section_count) {
    utl_config_section *sec = iterator->config->sections[iterator->section_index];
    if (iterator->entry_index < sec->entry_count) {
      *section = sec->section_name;
      *key = sec->entries[iterator->entry_index].key;
      *value = sec->entries[iterator->entry_index].value;
      iterator->entry_index++;
      return true;
    } 
    else {
      iterator->section_index++;
      iterator->entry_index = 0;
    }
  }
  return false;
}

void utl_config_reload(utl_config_file **config_ptr) {
  if (!config_ptr || !(*config_ptr) || !(*config_ptr)->filename) {
    utl_error_func("Invalid configuration pointer or missing filename", utl_user_defined_data);
    return;
  }
  utl_config_file *new_config = utl_config_create((*config_ptr)->filename);
  if (!new_config) {
    utl_error_func("Failed to reload configuration from file", utl_user_defined_data);
    return;
  }
  utl_config_deallocate(*config_ptr); 
  *config_ptr = new_config; 
}

void utl_config_register_modification_callback(utl_config_file *config, void (*callback)(const char *section, const char *key, const char *value)) {
  if (!config) {
    utl_error_func("Config file is null", utl_user_defined_data);
    return;
  }
  config->modification_callback = callback;
}

void utl_config_validate_structure(const utl_config_file *config, const utl_config_section *expected_structure, size_t structure_size) {
  if (!config || !expected_structure) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < structure_size; ++i) {
    const utl_config_section *expected_sec = &expected_structure[i];
    for (size_t j = 0; j < config->section_count; ++j) {
      if (strcmp(config->sections[j]->section_name, expected_sec->section_name) == 0) {
        break;
      }
    }
  }
}

char **utl_config_get_array(const utl_config_file *config, const char *section, const char *key, size_t array_size) {
  if (!config || !section || !key || !array_size) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    array_size = 0;
    return NULL;
  }
  const char *value = utl_config_get_value(config, section, key);
  if (!value) {
    array_size = 0;
    return NULL;
  }
  array_size = 1;
  for (const char *p = value; *p; ++p) {
    if (*p == ',') {
      (array_size)++;
    }
  }
  char **array = malloc(array_size * sizeof(char *)); 
  if (!array) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    array_size = 0;
    return NULL;
  }
  char *value_copy = utl_string_strdup(value);
  char *token = strtok(value_copy, ", ");
  size_t idx = 0;
  while (token) {
    array[idx++] = utl_string_strdup(token);
    token = strtok(NULL, ",");
  }
  free(value_copy);
  return array;
}

void utl_config_set_array(utl_config_file *config, const char *section, const char *key, const char *const *array, size_t array_size) {
  if (!config || !section || !key || !array || array_size == 0) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return;
  }
  size_t total_length = strlen(key) + 2;
  for (size_t i = 0; i < array_size; i++) {
    total_length += strlen(array[i]) + ((i < array_size - 1) ? 1 : 0); // +1 for comma, if not the last element
  }
  char *combined = malloc(total_length);
  if (!combined) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  char *ptr = combined;
  ptr += sprintf(ptr, "%s=", key);
  for (size_t i = 0; i < array_size; i++) {
    ptr += sprintf(ptr, "%s%s", array[i], (i < array_size - 1) ? ", " : "");
  }
  utl_config_set_value(config, section, key, combined);
  free(combined);
}

char *utl_config_get_encrypted_value(const utl_config_file *config, const char *section, const char *key, const char *encryption_key) {
  if (!config || !section || !key || !encryption_key) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return NULL;
  }
  const char *encrypted_value = utl_config_get_value(config, section, key);
  if (!encrypted_value) {
    utl_error_func("Unable to find encrypted value for key", utl_user_defined_data);
    return NULL;
  }
  size_t value_size = strlen(encrypted_value);
  char *decrypted_value = malloc(value_size + 1);
  if (!decrypted_value) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  utl_xor_encrypt_decrypt(encrypted_value, decrypted_value, encryption_key[0], value_size);
  decrypted_value[value_size] = '\0';
  return decrypted_value;
}

void utl_config_set_encrypted_value(utl_config_file *config, const char *section, const char *key, const char *value, const char *encryption_key) {
  if (!config || !section || !key || !value || !encryption_key) {
    utl_error_func("Invalid arguments provided", utl_user_defined_data);
    return;
  }
  size_t value_size = strlen(value);
  char *encrypted_value = malloc(value_size + 1);
  if (!encrypted_value) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  utl_xor_encrypt_decrypt(value, encrypted_value, encryption_key[0], value_size);
  encrypted_value[value_size] = '\0';
  utl_config_set_value(config, section, key, encrypted_value);
  free(encrypted_value);
}

