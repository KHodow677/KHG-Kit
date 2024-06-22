#pragma once

#include <stdbool.h>

void init_audio_device(void);
void close_audio_device(void);
bool is_audio_device_ready(void);
void set_master_volume(float volume);

