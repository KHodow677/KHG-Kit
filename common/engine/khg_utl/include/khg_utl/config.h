#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct utl_config_entry {
  bool is_comment;
  union {
    struct {
      char *key;
      char *value;
    };
    char *comment;
  };
} utl_config_entry;

typedef struct utl_config_section {
  char *section_name;
  utl_config_entry *entries;
  size_t entry_count;
  char *comment;
} utl_config_section;

typedef struct utl_config_file {
  utl_config_section **sections;
  size_t section_count;
  char *default_section;
  char *filename;
  void (*modification_callback)(const char *section, const char *key, const char *value);
} utl_config_file;

typedef struct utl_config_iterator {
  const utl_config_file *config;
  size_t section_index;
  size_t entry_index;
} utl_config_iterator;

utl_config_file *utl_config_create(const char *filename);

const char *utl_config_get_value(const utl_config_file *config, const char *section, const char *key);
char **utl_config_get_array(const utl_config_file *config, const char *section, const char *key, size_t array_size);
char *utl_config_get_encrypted_value(const utl_config_file *config, const char *section, const char *key, const char *encryption_key);

void utl_config_set_value(utl_config_file *config, const char *section, const char *key, const char *value);
void utl_config_remove_section(utl_config_file *config, const char *section);
void utl_config_remove_key(utl_config_file *config, const char *section, const char *key);
void utl_config_deallocate(utl_config_file *config);
void utl_config_save(const utl_config_file *config, const char *filename);
void utl_config_reload(utl_config_file **config);
void utl_config_register_modification_callback(utl_config_file *config, void (*callback)(const char *section, const char *key, const char *value));
void utl_config_validate_structure(const utl_config_file *config, const utl_config_section *expected_structure, size_t structure_size);
void utl_config_set_comment(utl_config_file *config, const char *section, const char *comment);
void utl_config_set_encrypted_value(utl_config_file *config, const char *section, const char *key, const char *value, const char *encryption_key);
void utl_config_set_array(utl_config_file *config, const char *section, const char *key, const char *const *array, size_t array_size);

bool utl_config_next_entry(utl_config_iterator *iterator, const char **section, const char **key, const char **value);
bool utl_config_get_bool(const utl_config_file *config, const char *section, const char *key, bool default_value);
bool utl_config_has_section(const utl_config_file *config, const char *section);
bool utl_config_has_key(const utl_config_file *config, const char *section, const char *key);

int utl_config_get_int(const utl_config_file *config, const char *section, const char *key, int default_value);
double utl_config_get_double(const utl_config_file *config, const char *section, const char *key, double default_value);
utl_config_iterator utl_config_get_iterator(const utl_config_file *config);

