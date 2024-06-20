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
  unsigned int sampleCount;
  unsigned int sampleRate;
  unsigned int sampleSize;
  unsigned int channels;
  void *data;
} wave;

typedef struct audioBuffer audioBuffer;

struct audioBuffer {
  ma_data_converter converter;
  float volume;
  float pitch;
  bool playing;
  bool paused;
  bool looping;
  int usage;
  bool isSubBufferProcessed[2];
  unsigned int sizeInFrames;
  unsigned int frameCursorPos;
  unsigned int totalFramesProcessed;
  unsigned char *data;
  audioBuffer *next;
  audioBuffer *prev;
};

typedef struct {
  unsigned int sampleRate;
  unsigned int sampleSize;
  unsigned int channels;
  audioBuffer *buffer;
} audioStream;

typedef struct {
  unsigned int sampleCount;
  audioStream stream;
} sound;

typedef struct {
  int ctxType;
  void *ctxData;
  bool looping;
  unsigned int sampleCount;
  audioStream stream;
} music;

typedef struct {
  struct {
    ma_context context;
    ma_device device;
    ma_mutex lock;
    bool isReady;
  } system;
  struct {
    audioBuffer *first;
    audioBuffer *last;
    int defaultSize;
  } buffer;
  struct {
    audioBuffer *pool[MAX_AUDIO_BUFFER_POOL_CHANNELS];
    unsigned int poolCounter;
    unsigned int channels[MAX_AUDIO_BUFFER_POOL_CHANNELS];
  } multiChannel;
} audioData;

typedef enum {
  MUSIC_AUDIO_WAV = 0,
  MUSIC_AUDIO_OGG,
  MUSIC_AUDIO_FLAC,
  MUSIC_AUDIO_MP3,
  MUSIC_MODULE_XM,
  MUSIC_MODULE_MOD
} musicContextType;

typedef enum {
  LOG_ALL,
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL,
  LOG_NONE
} traceLogType;

typedef enum {
  AUDIO_BUFFER_USAGE_STATIC = 0,
  AUDIO_BUFFER_USAGE_STREAM
} AudioBufferUsage;

extern audioData AUDIO;
