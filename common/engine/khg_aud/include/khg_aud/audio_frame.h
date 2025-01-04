#pragma once

#include "miniaudio/miniaudio.h"
#include <stdint.h>

void aud_mix_audio_frames(float *frames_out, const float *frames_in, unsigned int frame_count, float local_volume);

