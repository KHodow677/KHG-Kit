#include "khg_aud/audio_buffer.h"
#include "khg_aud/audio_frame.h"
#include "khg_aud/callbacks.h"
#include "khg_aud/utils.h"
#include "khg_utl/error_func.h"
#include <string.h>

void aud_on_log(ma_context *context, ma_device *device, ma_uint32 log_level, const char *message) {
  (void)context;
  (void)device;
  error_func("Miniaudio error", user_defined_data);
}

void aud_on_send_audio_data_to_device(ma_device *device, void *frames_out, const void *frames_input, ma_uint32 frame_count) {
  (void)device;
  memset(frames_out, 0, frame_count * device->playback.channels * ma_get_bytes_per_sample(device->playback.format));
  ma_mutex_lock(&audio.system.lock);
  for (aud_audio_buffer *a_buffer = audio.buffer.first; a_buffer != NULL; a_buffer = a_buffer->next) {
    if (!a_buffer->playing || a_buffer->paused) {
      continue;
    }
    ma_uint32 frames_read = 0;
    while (1) {
      if (frames_read >= frame_count) {
        break;
      }
      ma_uint32 frames_to_read = (frame_count - frames_read);
      while (frames_to_read > 0) {
        float temp_buffer[1024];
        ma_uint32 frames_to_read_right_now = frames_to_read;
        if (frames_to_read_right_now > sizeof(temp_buffer) / sizeof(temp_buffer[0]) / AUD_AUDIO_DEVICE_CHANNELS) {
          frames_to_read_right_now = sizeof(temp_buffer) / sizeof(temp_buffer[0]) / AUD_AUDIO_DEVICE_CHANNELS;
        }
        ma_uint32 frames_just_read = aud_read_audio_buffer_frames_in_mixing_format(a_buffer, temp_buffer, frames_to_read_right_now);
        if (frames_just_read > 0) {
          float *frames_out = (float *)frames_out + (frames_read * audio.system.device.playback.channels);
          float *frames_in  = temp_buffer;
          aud_mix_audio_frames(frames_out, frames_in, frames_just_read, a_buffer->volume);
          frames_to_read -= frames_just_read;
          frames_read += frames_just_read;
        }
        if (!a_buffer->playing) {
          frames_read = frame_count;
          break;
        }
        if (frames_just_read < frames_to_read_right_now) {
          if (!a_buffer->looping) {
            aud_stop_audio_buffer(a_buffer);
            break;
          }
          else {
            a_buffer->frame_cursor_pos = 0;
            continue;
          }
        }
      }
      if (frames_to_read > 0) break;
    }
  }
  ma_mutex_unlock(&audio.system.lock);
}
