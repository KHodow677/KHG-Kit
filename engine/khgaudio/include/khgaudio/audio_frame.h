#pragma once

#include "miniaudio/miniaudio.h"

void mix_audio_frames(float *frames_out, const float *frames_in, ma_uint32 frame_count, float local_volume);
