#include "rig/anim.h"
#include "khg_utl/config.h"
#include <stdio.h>
#include <unistd.h>

bool generate_animation_frame(const char *dir_path, const int anim_num, const int frame_num) {
  char file[256];
  snprintf(file, 256, "%s%i.ini", dir_path, frame_num);
  if (access(file, F_OK) == -1) {
    return false;
  }
  printf("Loaded %s\n", file);
  utl_config_file *config = utl_config_create(file);
  frame_info new_frame;
  utl_config_deallocate(config);
  return true;
}

void generate_animation_from_path(const char *dir_path, const int anim_num) {
  int count = 0;
  while (generate_animation_frame(dir_path, anim_num, count)) {
    count++;
  }
  printf("Loaded Frames");
}

