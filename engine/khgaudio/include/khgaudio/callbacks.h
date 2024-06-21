#pragma once

#include "miniaudio/miniaudio.h"

void on_log(ma_context *context, ma_device *device, ma_uint32 log_level, const char *message);
void on_send_audio_data_to_device(ma_device *device, void *frames_out, const void *frames_input, ma_uint32 frame_count);
