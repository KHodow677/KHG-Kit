#pragma once

#include "khgaudio/utils.h"
#include "miniaudio/miniaudio.h"

audio_buffer *load_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 size_in_frames, int usage);
void unload_audio_buffer(audio_buffer *b);
bool is_audio_buffer_playing(audio_buffer *b);
void play_audio_buffer(audio_buffer *b);
void stop_audio_buffer(audio_buffer *b);
void pause_audio_buffer(audio_buffer *b);
void resume_audio_buffer(audio_buffer *b);
void set_audio_buffer_volume(audio_buffer *b, float volume);
void set_audio_buffer_pitch(audio_buffer *b, float pitch);
void track_audio_buffer(audio_buffer *b);
void untrack_audio_buffer(audio_buffer *b);
ma_uint32 read_audio_buffer_frames_in_internal_format(audio_buffer *b, void *frames_out, ma_uint32 frame_count);
ma_uint32 read_audio_buffer_frames_in_mixing_format(audio_buffer *b, float *frames_out, ma_uint32 frame_count);
void init_audio_buffer_pool(void);
void close_audio_buffer_pool(void);

