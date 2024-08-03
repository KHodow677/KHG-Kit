#include "khg_aud/audio_buffer.h"
#include "khg_aud/sound_management.h"
#include "khg_aud/utils.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <string.h>

void aud_play_sound(aud_sound s) {
  aud_play_audio_buffer(s.stream.buffer);
}

void aud_stop_sound(aud_sound s) {
  aud_stop_audio_buffer(s.stream.buffer);
}

void aud_pause_sound(aud_sound s) {
  aud_pause_audio_buffer(s.stream.buffer);
}

void aud_resume_sound(aud_sound s) {
  aud_resume_audio_buffer(s.stream.buffer);
}

void aud_play_sound_multi(aud_sound s) {
  int index = -1;
  unsigned int old_age = 0;
  int old_index = -1;
  for (int i = 0; i < AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) {
    if (audio.multi_channel.channels[i] > old_age) {
      old_age = audio.multi_channel.channels[i];
      old_index = i;
    }
    if (!aud_is_audio_buffer_playing(audio.multi_channel.pool[i])) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    error_func("Buffer pool is already full", user_defined_data);
    if (old_index == -1) {
      error_func("Buffer pool could not determine oldest buffer not playing sound", user_defined_data);
      return;
    }
    index = old_index;
    aud_stop_audio_buffer(audio.multi_channel.pool[index]);
  }
  audio.multi_channel.channels[index] = audio.multi_channel.pool_counter;
  audio.multi_channel.pool_counter++;
  audio.multi_channel.pool[index]->volume = s.stream.buffer->volume;
  audio.multi_channel.pool[index]->pitch = s.stream.buffer->pitch;
  audio.multi_channel.pool[index]->looping = s.stream.buffer->looping;
  audio.multi_channel.pool[index]->usage = s.stream.buffer->usage;
  audio.multi_channel.pool[index]->is_sub_buffer_processed[0] = false;
  audio.multi_channel.pool[index]->is_sub_buffer_processed[1] = false;
  audio.multi_channel.pool[index]->size_in_frames = s.stream.buffer->size_in_frames;
  audio.multi_channel.pool[index]->data = s.stream.buffer->data;
  aud_play_audio_buffer(audio.multi_channel.pool[index]);
}

void aud_stop_sound_multi(void) {
  for (int i = 0; i < AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) {
    aud_stop_audio_buffer(audio.multi_channel.pool[i]);
  }
}

int aud_get_sounds_playing(void) {
  int counter = 0;
  for (int i = 0; i < AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) {
    if (aud_is_audio_buffer_playing(audio.multi_channel.pool[i])) {
      counter++;
    }
  }
  return counter;
}

bool aud_is_sound_playing(aud_sound s) {
  return aud_is_audio_buffer_playing(s.stream.buffer);
}

void aud_set_sound_volume(aud_sound s, float volume) {
  aud_set_audio_buffer_volume(s.stream.buffer, volume);
}

void aud_set_sound_pitch(aud_sound s, float pitch) {
  aud_set_audio_buffer_pitch(s.stream.buffer, pitch);
}

void aud_wave_format(aud_wave *w, int sample_rate, int sample_size, int channels) {
  ma_format format_in  = ((w->sample_size == 8)? ma_format_u8 : ((w->sample_size == 16)? ma_format_s16 : ma_format_f32));
  ma_format format_out = ((sample_size == 8)? ma_format_u8 : ((sample_size == 16)? ma_format_s16 : ma_format_f32));
  ma_uint32 frame_count_in = w->sample_count;
  ma_uint32 frame_count = (ma_uint32)ma_convert_frames(NULL, 0, format_out, channels, sample_rate, NULL, frame_count_in, format_in, w->channels, w->sample_rate);
  if (frame_count == 0) {
    error_func("Failed to get frame count for format conversion", user_defined_data);
    return;
  }
  void *data = malloc(frame_count * channels * (sample_size / 8));
  frame_count = (ma_uint32)ma_convert_frames(data, frame_count, format_out, channels, sample_rate, w->data, frame_count_in, format_in, w->channels, w->sample_rate);
  if (frame_count == 0) {
    error_func("Failed format conversion", user_defined_data);
    return;
  }
  w->sample_count = frame_count;
  w->sample_size = sample_size;
  w->sample_rate = sample_rate;
  w->channels = channels;
  free(w->data);
  w->data = data;
}

aud_wave aud_wave_copy(aud_wave w) {
  aud_wave newWave = { 0 };
  newWave.data = malloc(w.sample_count * w.sample_size / 8 * w.channels);
  if (newWave.data != NULL) {
    memcpy(newWave.data, w.data, w.sample_count * w.channels * w.sample_size / 8);
    newWave.sample_count = w.sample_count;
    newWave.sample_rate = w.sample_rate;
    newWave.sample_size = w.sample_size;
    newWave.channels = w.channels;
  }
  return newWave;
}

void aud_wave_crop(aud_wave *w, int init_sample, int final_sample) {
  if ((init_sample >= 0) && (init_sample < final_sample) && (final_sample > 0) && ((unsigned int)final_sample < w->sample_count)) {
    int sample_count = final_sample - init_sample;
    void *data = malloc(sample_count * w->sample_size / 8 * w->channels);
    memcpy(data, (unsigned char *)w->data + (init_sample * w->channels * w->sample_size / 8), sample_count * w->channels * w->sample_size / 8);
    free(w->data);
    w->data = data;
  }
  else {
    error_func("WAVE: Crop range out of bounds", user_defined_data);
  }
}

float *aud_get_wave_data(aud_wave w) {
  float *samples = (float *)malloc(w.sample_count * w.channels*sizeof(float));
  for (unsigned int i = 0; i < w.sample_count; i++) {
    for (unsigned int j = 0; j < w.channels; j++) {
      if (w.sample_size == 8) samples[w.channels * i + j] = (float)(((unsigned char *)w.data)[w.channels * i + j] - 127) / 256.0f;
      else if (w.sample_size == 16) samples[w.channels * i + j] = (float)((short *)w.data)[w.channels * i + j] / 32767.0f;
      else if (w.sample_size == 32) samples[w.channels * i + j] = ((float *)w.data)[w.channels * i + j];
    }
  }
  return samples;
}
