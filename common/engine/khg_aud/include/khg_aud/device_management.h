#pragma once

#include <stdbool.h>

void aud_init_audio_device(void);
void aud_close_audio_device(void);
bool aud_is_audio_device_ready(void);
void aud_set_master_volume(float volume);

