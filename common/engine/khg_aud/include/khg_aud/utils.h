#pragma once

#include "miniaudio/miniaudio.h"
#include <stdbool.h>

#define AUD_AUDIO_DEVICE_FORMAT ma_format_f32
#define AUD_AUDIO_DEVICE_CHANNELS 2
#define AUD_AUDIO_DEVICE_SAMPLE_RATE 44100
#define AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS 16
#define AUD_DEFAULT_AUDIO_BUFFER_SIZE 4096

typedef struct {
  unsigned int sample_count;
  unsigned int sample_rate;
  unsigned int sample_size;
  unsigned int channels;
  void *data;
} aud_wave;

typedef struct aud_audio_buffer aud_audio_buffer;

struct aud_audio_buffer {
  ma_data_converter converter;
  float volume;
  float pitch;
  bool playing;
  bool paused;
  bool looping;
  int usage;
  bool is_sub_buffer_processed[2];
  unsigned int size_in_frames;
  unsigned int frame_cursor_pos;
  unsigned int total_frames_processed;
  unsigned char *data;
  aud_audio_buffer *next;
  aud_audio_buffer *prev;
};

typedef struct {
  unsigned int sample_rate;
  unsigned int sample_size;
  unsigned int channels;
  aud_audio_buffer *buffer;
} aud_audio_stream;

typedef struct {
  unsigned int sample_count;
  aud_audio_stream stream;
} aud_sound;

typedef struct {
  int ctx_type;
  void *ctx_data;
  bool looping;
  unsigned int sample_count;
  aud_audio_stream stream;
} aud_music;

typedef struct {
  struct {
    ma_context context;
    ma_device device;
    ma_mutex lock;
    bool is_ready;
  } system;
  struct {
    aud_audio_buffer *first;
    aud_audio_buffer *last;
    int default_size;
  } buffer;
  struct {
    aud_audio_buffer *pool[AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS];
    unsigned int pool_counter;
    unsigned int channels[AUD_MAX_AUDIO_BUFFER_POOL_CHANNELS];
  } multi_channel;
} aud_audio_data;

typedef enum {
  AUD_MUSIC_AUDIO_WAV = 0,
  AUD_MUSIC_AUDIO_OGG,
  AUD_MUSIC_AUDIO_FLAC,
  AUD_MUSIC_AUDIO_MP3,
  AUD_MUSIC_MODULE_XM,
  AUD_MUSIC_MODULE_MOD
} aud_music_context;

typedef enum {
  AUD_AUDIO_BUFFER_USAGE_STATIC = 0,
  AUD_AUDIO_BUFFER_USAGE_STREAM
} aud_audio_buffer_usage;

extern aud_audio_data AUD_AUDIO;

