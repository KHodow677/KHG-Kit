#include "khg_aud/audio_buffer.h"
#include "khg_aud/audio_stream_management.h"
#include "khg_utl/error_func.h"
#include <string.h>

aud_audio_stream aud_init_audio_stream(unsigned int sample_rate, unsigned int sample_size, unsigned int channels) {
  aud_audio_stream stream = { 0 };
  stream.sample_rate = sample_rate;
  stream.sample_size = sample_size;
  stream.channels = channels;
  ma_format format_in = ((stream.sample_size == 8)? ma_format_u8 : ((stream.sample_size == 16)? ma_format_s16 : ma_format_f32));
  unsigned int period_size = audio.system.device.playback.internalPeriodSizeInFrames;
  unsigned int sub_buffer_size = audio.buffer.default_size;
  if (sub_buffer_size < period_size) sub_buffer_size = period_size;
  stream.buffer = aud_load_audio_buffer(format_in, stream.channels, stream.sample_rate, sub_buffer_size*2, AUD_AUDIO_BUFFER_USAGE_STREAM);
  if (stream.buffer != NULL) {
    stream.buffer->looping = true;
  }
  else {
    error_func("Failed to load audio buffer, stream could not be created", user_defined_data);
  }
  return stream;
}

void aud_update_audio_stream(aud_audio_stream stream, const void *data, int samples_count) {
  if (stream.buffer != NULL) {
    if (stream.buffer->is_sub_buffer_processed[0] || stream.buffer->is_sub_buffer_processed[1]) {
      ma_uint32 subBufferToUpdate = 0;
      if (stream.buffer->is_sub_buffer_processed[0] && stream.buffer->is_sub_buffer_processed[1]) {
        subBufferToUpdate = 0;
        stream.buffer->frame_cursor_pos = 0;
      }
      else {
        subBufferToUpdate = (stream.buffer->is_sub_buffer_processed[0])? 0 : 1;
      }
      ma_uint32 subBufferSizeInFrames = stream.buffer->size_in_frames / 2;
      unsigned char *subBuffer = stream.buffer->data + ((subBufferSizeInFrames*stream.channels*(stream.sample_size / 8)) * subBufferToUpdate);
      stream.buffer->total_frames_processed += subBufferSizeInFrames;
      if (subBufferSizeInFrames >= (ma_uint32)samples_count / stream.channels) {
        ma_uint32 framesToWrite = subBufferSizeInFrames;
        if (framesToWrite > ((ma_uint32)samples_count / stream.channels)) {
          framesToWrite = (ma_uint32)samples_count / stream.channels;
        }
        ma_uint32 bytesToWrite = framesToWrite*stream.channels*(stream.sample_size / 8);
        memcpy(subBuffer, data, bytesToWrite);
        ma_uint32 leftoverFrameCount = subBufferSizeInFrames - framesToWrite;
        if (leftoverFrameCount > 0) {
          memset(subBuffer + bytesToWrite, 0, leftoverFrameCount*stream.channels * (stream.sample_size / 8));
        }
        stream.buffer->is_sub_buffer_processed[subBufferToUpdate] = false;
      }
      else {
        error_func("Attempting to write too many frames to buffer", user_defined_data);
      }
    }
    else {
      error_func("Buffer not available for updating", user_defined_data);
    }
  }
}

void aud_close_audio_stream(aud_audio_stream stream) {
  aud_unload_audio_buffer(stream.buffer);
}

bool aud_is_audio_stream_processed(aud_audio_stream stream) {
  if (stream.buffer == NULL) {
    return false;
  }
  return (stream.buffer->is_sub_buffer_processed[0] || stream.buffer->is_sub_buffer_processed[1]);
}

void aud_play_audio_stream(aud_audio_stream stream) {
  aud_play_audio_buffer(stream.buffer);
}

void aud_pause_audio_stream(aud_audio_stream stream) {
  aud_pause_audio_buffer(stream.buffer);
}

void aud_resume_audio_stream(aud_audio_stream stream) {
  aud_resume_audio_buffer(stream.buffer);
}

bool aud_is_audio_stream_playing(aud_audio_stream stream) {
  return aud_is_audio_buffer_playing(stream.buffer);
}

void aud_stop_audio_stream(aud_audio_stream stream) {
  aud_stop_audio_buffer(stream.buffer);
}

void aud_set_audio_stream_volume(aud_audio_stream stream, float volume) {
  aud_set_audio_buffer_volume(stream.buffer, volume);
}

void aud_set_audio_stream_pitch(aud_audio_stream stream, float pitch) {
  aud_set_audio_buffer_pitch(stream.buffer, pitch);
}

void aud_set_audiostream_buffer_size_default(int size) {
  audio.buffer.default_size = size;
}
