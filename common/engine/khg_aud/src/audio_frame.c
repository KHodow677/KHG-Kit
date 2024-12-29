#include "khg_aud/audio_frame.h"
#include "khg_aud/utils.h"
#include <stdint.h>

void aud_mix_audio_frames(float *frames_out, const float *frames_in, unsigned int frame_count, float local_volume) {
  for (unsigned int i_frame = 0; i_frame < frame_count; ++i_frame) {
    for (unsigned int i_channel = 0; i_channel < AUD_AUDIO.system.device.playback.channels; ++i_channel) {
      float *frame_out = frames_out + (i_frame * AUD_AUDIO.system.device.playback.channels);
      const float *frame_in  = frames_in  + (i_frame * AUD_AUDIO.system.device.playback.channels);
      frame_out[i_channel] += (frame_in[i_channel] * local_volume);
    }
  }
}
