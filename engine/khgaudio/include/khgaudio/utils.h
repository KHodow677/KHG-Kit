#pragma once

#define SUPPORT_FILEFORMAT_WAV
#define SUPPORT_FILEFORMAT_OGG
#define SUPPORT_FILEFORMAT_FLAC
#define SUPPORT_FILEFORMAT_MP3
#define SUPPORT_FILEFORMAT_XM
#define SUPPORT_FILEFORMAT_MOD

#include "miniaudio/miniaudio.h"

#ifndef __cplusplus
// Boolean type
    #if !defined(_STDBOOL_H)
        typedef enum { false, true } bool;
        #define _STDBOOL_H
    #endif
#endif

#define AUDIO_DEVICE_FORMAT ma_format_f32
#define AUDIO_DEVICE_CHANNELS 2
#define AUDIO_DEVICE_SAMPLE_RATE 44100
#define MAX_AUDIO_BUFFER_POOL_CHANNELS 16
#define DEFAULT_AUDIO_BUFFER_SIZE 4096
#define TRACELOG(level, ...) (void)0

typedef struct {
  unsigned int sample_count;
  unsigned int sample_rate;
  unsigned int sample_size;
  unsigned int channels;
  void *data;
} wave;

typedef struct audio_buffer audio_buffer;

struct audio_buffer {
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
  audio_buffer *next;
  audio_buffer *prev;
};

typedef struct {
  unsigned int sample_rate;
  unsigned int sample_size;
  unsigned int channels;
  audio_buffer *buffer;
} audio_stream;

typedef struct {
  unsigned int sample_count;
  audio_stream stream;
} sound;

typedef struct {
  int ctx_type;
  void *ctx_data;
  bool looping;
  unsigned int sample_count;
  audio_stream stream;
} music;

typedef struct {
  struct {
    ma_context context;
    ma_device device;
    ma_mutex lock;
    bool is_ready;
  } system;
  struct {
    audio_buffer *first;
    audio_buffer *last;
    int default_size;
  } buffer;
  struct {
    audio_buffer *pool[MAX_AUDIO_BUFFER_POOL_CHANNELS];
    unsigned int pool_counter;
    unsigned int channels[MAX_AUDIO_BUFFER_POOL_CHANNELS];
  } multi_channel;
} audio_data;

typedef enum {
  MUSIC_AUDIO_WAV = 0,
  MUSIC_AUDIO_OGG,
  MUSIC_AUDIO_FLAC,
  MUSIC_AUDIO_MP3,
  MUSIC_MODULE_XM,
  MUSIC_MODULE_MOD
} music_context_type;

typedef enum {
  LOG_ALL,
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL,
  LOG_NONE
} trace_log_type;

typedef enum {
  AUDIO_BUFFER_USAGE_STATIC = 0,
  AUDIO_BUFFER_USAGE_STREAM
} audio_buffer_usage;

extern audio_data AUDIO;
