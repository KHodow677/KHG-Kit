#pragma once

#include <stdbool.h>

extern volatile bool RESOUCES_LOADED;
extern volatile unsigned int TEXTURE_LOAD_PROGRESS;
extern volatile unsigned int OVR_TILE_LOAD_PROGRESS; 

void load_resources_defer(const unsigned int batch_size);

