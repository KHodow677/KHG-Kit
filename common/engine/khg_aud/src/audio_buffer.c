#include "khg_aud/audio_buffer.h"
#include "khg_utl/error_func.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

aud_audio_buffer *aud_load_audio_buffer(ma_format format, unsigned int channels, unsigned int sample_rate, unsigned int size_in_frames, int usage) {
  aud_audio_buffer *a_buffer = (aud_audio_buffer *)calloc(1, sizeof(aud_audio_buffer));
  if (a_buffer == NULL) {
    utl_error_func("Failed to allocate memory for buffer", utl_user_defined_data);
    return NULL;
  }
  if (size_in_frames > 0) {
    a_buffer->data = calloc(size_in_frames * channels * ma_get_bytes_per_sample(format), 1);
  }
  ma_data_converter_config converter_config = ma_data_converter_config_init(format, AUD_AUDIO_DEVICE_FORMAT, channels, AUD_AUDIO_DEVICE_CHANNELS, sample_rate, AUD_AUDIO_DEVICE_SAMPLE_RATE);
  converter_config.resampling.allowDynamicSampleRate = true;
  ma_result result = ma_data_converter_init(&converter_config, &a_buffer->converter);
  if (result != MA_SUCCESS) {
    utl_error_func("Failed to create data conversion pipeline", utl_user_defined_data);
    free(a_buffer);
    return NULL;
  }
  a_buffer->volume = 1.0f;
  a_buffer->pitch = 1.0f;
  a_buffer->playing = false;
  a_buffer->paused = false;
  a_buffer->looping = false;
  a_buffer->usage = usage;
  a_buffer->frame_cursor_pos = 0;
  a_buffer->size_in_frames = size_in_frames;
  a_buffer->is_sub_buffer_processed[0] = true;
  a_buffer->is_sub_buffer_processed[1] = true;
  aud_track_audio_buffer(a_buffer);
  return a_buffer;
}

void aud_unload_audio_buffer(aud_audio_buffer *b) {
  if (b != NULL) {
    ma_data_converter_uninit(&b->converter);
    aud_untrack_audio_buffer(b);
    free(b->data);
    free(b);
  }
}

bool aud_is_audio_buffer_playing(aud_audio_buffer *b) {
  bool result = false;
  if (b != NULL) {
    result = (b->playing && !b->paused);
  }
  return result;
}

void aud_play_audio_buffer(aud_audio_buffer *b) {
  if (b != NULL) {
    b->playing = true;
    b->paused = false;
    b->frame_cursor_pos = 0;
  }
}

void aud_stop_audio_buffer(aud_audio_buffer *b) {
  if (b != NULL) {
    if (aud_is_audio_buffer_playing(b)) {
      b->playing = false;
      b->paused = false;
      b->frame_cursor_pos = 0;
      b->total_frames_processed = 0;
      b->is_sub_buffer_processed[0] = true;
      b->is_sub_buffer_processed[1] = true;
    }
  }
}

void aud_pause_audio_buffer(aud_audio_buffer *b) {
  if (b != NULL) {
    b->paused = true;
  }
}

void aud_resume_audio_buffer(aud_audio_buffer *b) {
  if (b != NULL) {
    b->paused = false;
  }
}

void aud_set_audio_buffer_volume(aud_audio_buffer *b, float volume) {
  if (b != NULL) { 
    b->volume = volume;
  }
}

void aud_set_audio_buffer_pitch(aud_audio_buffer *b, float pitch) {
  if (b != NULL) {
    float pitch_mul = pitch/b->pitch;
    unsigned int new_output_sample_rate = (unsigned int)((float)b->converter.config.sampleRateOut/pitch_mul);
    b->pitch *= (float)b->converter.config.sampleRateOut/new_output_sample_rate;
    ma_data_converter_set_rate(&b->converter, b->converter.config.sampleRateIn, new_output_sample_rate);
  }
}

void aud_track_audio_buffer(aud_audio_buffer *b) {
  ma_mutex_lock(&AUD_AUDIO.system.lock);
  if (AUD_AUDIO.buffer.first == NULL) {
    AUD_AUDIO.buffer.first = b;
  }
  else {
    AUD_AUDIO.buffer.last->next = b;
    b->prev = AUD_AUDIO.buffer.last;
  }
  AUD_AUDIO.buffer.last = b;
  ma_mutex_unlock(&AUD_AUDIO.system.lock);
}

void aud_untrack_audio_buffer(aud_audio_buffer *b) {
  ma_mutex_lock(&AUD_AUDIO.system.lock);
  if (b->prev == NULL) {
    AUD_AUDIO.buffer.first = b->next;
  }
  else {
    b->prev->next = b->next;
  }
  if (b->next == NULL) {
    AUD_AUDIO.buffer.last = b->prev;
  }
  else {
    b->next->prev = b->prev;
  }
  b->prev = NULL;
  b->next = NULL;
  ma_mutex_unlock(&AUD_AUDIO.system.lock);
}

unsigned int aud_read_audio_buffer_frames_in_internal_format(aud_audio_buffer *b, void *frames_out, unsigned int frame_count) {
  unsigned int sub_buffer_size_in_frames = (b->size_in_frames > 1)? b->size_in_frames/2 : b->size_in_frames;
  unsigned int current_sub_buffer_index = b->frame_cursor_pos/sub_buffer_size_in_frames;
  if (current_sub_buffer_index > 1){
    return 0;
  }
  bool is_sub_buffer_processed[2];
  is_sub_buffer_processed[0] = b->is_sub_buffer_processed[0];
  is_sub_buffer_processed[1] = b->is_sub_buffer_processed[1];
  unsigned int frame_size_in_bytes = ma_get_bytes_per_frame(b->converter.config.formatIn, b->converter.config.channelsIn);
  unsigned int frames_read = 0;
  while (1) {
    if (b->usage == AUD_AUDIO_BUFFER_USAGE_STATIC) {
      if (frames_read >= frame_count) {
        break;
      }
    }
    else {
      if (is_sub_buffer_processed[current_sub_buffer_index]) {
        break;
      }
    }
    unsigned int total_frames_remaining = (frame_count - frames_read);
    if (total_frames_remaining == 0) {
      break;
    }
    unsigned int frames_remaining_in_output_buffer;
    if (b->usage == AUD_AUDIO_BUFFER_USAGE_STATIC) {
      frames_remaining_in_output_buffer = b->size_in_frames - b->frame_cursor_pos;
    }
    else {
      unsigned int firstFrameIndexOfThisSubBuffer = sub_buffer_size_in_frames*current_sub_buffer_index;
      frames_remaining_in_output_buffer = sub_buffer_size_in_frames - (b->frame_cursor_pos - firstFrameIndexOfThisSubBuffer);
    }
    unsigned int frames_to_read = total_frames_remaining;
    if (frames_to_read > frames_remaining_in_output_buffer) {
      frames_to_read = frames_remaining_in_output_buffer;
    }
    memcpy((unsigned char *)frames_out + (frames_read*frame_size_in_bytes), b->data + (b->frame_cursor_pos*frame_size_in_bytes), frames_to_read*frame_size_in_bytes);
    b->frame_cursor_pos = (b->frame_cursor_pos + frames_to_read)%b->size_in_frames;
    frames_read += frames_to_read;
    if (frames_to_read == frames_remaining_in_output_buffer) {
      b->is_sub_buffer_processed[current_sub_buffer_index] = true;
      is_sub_buffer_processed[current_sub_buffer_index] = true;
      current_sub_buffer_index = (current_sub_buffer_index + 1)%2;
      if (!b->looping) {
        aud_stop_audio_buffer(b);
        break;
      }
    }
  }
  unsigned int total_frames_remaining = (frame_count - frames_read);
  if (total_frames_remaining > 0) {
    memset((unsigned char *)frames_out + (frames_read*frame_size_in_bytes), 0, total_frames_remaining*frame_size_in_bytes);
    if (b->usage != AUD_AUDIO_BUFFER_USAGE_STATIC) {
      frames_read += total_frames_remaining;
    }
  }
  return frames_read;
}

unsigned int aud_read_audio_buffer_frames_in_mixing_format(aud_audio_buffer *b, float *frames_out, unsigned int frame_count) {
  char input_buffer[4096];
  unsigned int input_buffer_frame_cap = sizeof(input_buffer) / ma_get_bytes_per_frame(b->converter.config.formatIn, b->converter.config.channelsIn);
  unsigned int total_output_frames_processed = 0;
  while (total_output_frames_processed < frame_count) {
    unsigned long long output_frames_to_process_this_iteration = frame_count - total_output_frames_processed;
    unsigned long long input_frames_to_process_this_iteration = ma_data_converter_get_required_input_frame_count(&b->converter, output_frames_to_process_this_iteration);
    if (input_frames_to_process_this_iteration > input_buffer_frame_cap) {
      input_frames_to_process_this_iteration = input_buffer_frame_cap;
    }
    float *running_frames_out = frames_out + (total_output_frames_processed * b->converter.config.channelsOut);
    unsigned long input_frames_processed_this_iteration = aud_read_audio_buffer_frames_in_internal_format(b, input_buffer, (unsigned int)input_frames_to_process_this_iteration); 
    unsigned long output_frames_processed_this_iteration = output_frames_to_process_this_iteration;
    ma_data_converter_process_pcm_frames(&b->converter, input_buffer, &input_frames_processed_this_iteration, running_frames_out, &output_frames_processed_this_iteration);
    total_output_frames_processed += (unsigned int)output_frames_processed_this_iteration;
    if (input_frames_processed_this_iteration < input_frames_to_process_this_iteration) {
      break;
    }
    if (input_frames_processed_this_iteration == 0 && output_frames_processed_this_iteration == 0) {
      break;
    }
  }
  return total_output_frames_processed;
}

void aud_init_audio_buffer_pool(void) {
  for (int i = 0; i < AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) {
    AUD_AUDIO.multi_channel.pool[i] = aud_load_audio_buffer(AUD_AUDIO_DEVICE_FORMAT, AUD_AUDIO_DEVICE_CHANNELS, AUD_AUDIO_DEVICE_SAMPLE_RATE, 0, AUD_AUDIO_BUFFER_USAGE_STATIC);
  }
}

void aud_close_audio_buffer_pool(void) {
  for (int i = 0; i < AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) {
    free(AUD_AUDIO.multi_channel.pool[i]);
  }
}
