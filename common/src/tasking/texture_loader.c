#include "khg_utl/config.h"
#include "khg_utl/vector.h"
#include "tasking/namespace.h"
#include <string.h>

static utl_vector *TEXTURE_DATA;

void populate_texture_data(const char *filename) {
  TEXTURE_DATA = utl_vector_create(sizeof(texture_object));
  utl_config_file *config = utl_config_create(filename);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  char last_section[128] = "";
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (!strcmp(last_section, section)) {
      strcpy(last_section, section);
      continue;
    };
    strcpy(last_section, section);
    const char *path = utl_config_get_value(config, section, "path");
    const int width = utl_config_get_int(config, section, "width", 512);
    const int height = utl_config_get_int(config, section, "height", 512);
    texture_object tex_obj = { .loaded = false };
    strcpy(tex_obj.name, section);
    strcpy(tex_obj.path, path);
    tex_obj.texture.width = width;
    tex_obj.texture.height = height;
    utl_vector_push_back(TEXTURE_DATA, &tex_obj);
  }
  utl_config_deallocate(config);
}

void clear_texture_data() {
  utl_vector_deallocate(TEXTURE_DATA);
}

