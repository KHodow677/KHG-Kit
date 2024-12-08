#pragma once

#include "rig/rig.h"

rig_builder generate_rig_builder_from_file(const char *filepath, const char *section, const int num_anim);
void generate_rig_from_file(rig *r, const char *filepath, const char *rig_section);

