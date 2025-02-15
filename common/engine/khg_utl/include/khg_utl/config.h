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
  unsigned int entry_count;
  char *comment;
} utl_config_section;

typedef struct utl_config_file {
  utl_config_section **sections;
  unsigned int section_count;
  char *default_section;
  char *filename;
  void (*modification_callback)(const char *section, const char *key, const char *value);
} utl_config_file;

typedef struct utl_config_iterator {
  const utl_config_file *config;
  unsigned int section_index;
  unsigned int entry_index;
} utl_config_iterator;

utl_config_file *utl_config_create(const char *filename);

const char *utl_config_get_value(const utl_config_file *config, const char *section, const char *key);
char **utl_config_get_array(const utl_config_file *config, const char *section, const char *key, unsigned int array_size);
char *utl_config_get_encrypted_value(const utl_config_file *config, const char *section, const char *key, const char *encryption_key);

void utl_config_set_value(utl_config_file *config, const char *section, const char *key, const char *value);
void utl_config_remove_section(utl_config_file *config, const char *section);
void utl_config_remove_key(utl_config_file *config, const char *section, const char *key);
void utl_config_deallocate(utl_config_file *config);
void utl_config_save(const utl_config_file *config, const char *filename);
void utl_config_reload(utl_config_file **config);
void utl_config_register_modification_callback(utl_config_file *config, void (*callback)(const char *section, const char *key, const char *value));
void utl_config_validate_structure(const utl_config_file *config, const utl_config_section *expected_structure, unsigned int structure_size);
void utl_config_set_comment(utl_config_file *config, const char *section, const char *comment);
void utl_config_set_encrypted_value(utl_config_file *config, const char *section, const char *key, const char *value, const char *encryption_key);
void utl_config_set_array(utl_config_file *config, const char *section, const char *key, const char *const *array, unsigned int array_size);

bool utl_config_next_entry(utl_config_iterator *iterator, const char **section, const char **key, const char **value);
bool utl_config_get_bool(const utl_config_file *config, const char *section, const char *key, bool default_value);
bool utl_config_has_section(const utl_config_file *config, const char *section);
bool utl_config_has_key(const utl_config_file *config, const char *section, const char *key);

int utl_config_get_int(const utl_config_file *config, const char *section, const char *key, int default_value);
float utl_config_get_float(const utl_config_file *config, const char *section, const char *key, float default_value);
utl_config_iterator utl_config_get_iterator(const utl_config_file *config);

