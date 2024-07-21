#include "khg_aud/audio_frame.h"
#include "khg_aud/utils.h"

void mix_audio_frames(float *frames_out, const float *frames_in, ma_uint32 frame_count, float local_volume) {
  for (ma_uint32 i_frame = 0; i_frame < frame_count; ++i_frame) {
    for (ma_uint32 i_channel = 0; i_channel < audio.system.device.playback.channels; ++i_channel) {
      float *frame_out = frames_out + (i_frame * audio.system.device.playback.channels);
      const float *frame_in  = frames_in  + (i_frame * audio.system.device.playback.channels);
      frame_out[i_channel] += (frame_in[i_channel] * local_volume);
    }
  }
}
