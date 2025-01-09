#pragma once

#include "miniaudio/miniaudio.h"

void aud_on_log(ma_context *context, ma_device *device, unsigned int log_level, const char *message);
void aud_on_send_audio_data_to_device(ma_device *device, void *frames_out, const void *frames_input, unsigned int frame_count);

