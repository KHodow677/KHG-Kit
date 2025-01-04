#pragma once

#include "khg_aud/utils.h"
#include "miniaudio/miniaudio.h"

aud_audio_buffer *aud_load_audio_buffer(ma_format format, unsigned int channels, unsigned int sample_rate, unsigned int size_in_frames, int usage);
void aud_unload_audio_buffer(aud_audio_buffer *b);
bool aud_is_audio_buffer_playing(aud_audio_buffer *b);
void aud_play_audio_buffer(aud_audio_buffer *b);
void aud_stop_audio_buffer(aud_audio_buffer *b);
void aud_pause_audio_buffer(aud_audio_buffer *b);
void aud_resume_audio_buffer(aud_audio_buffer *b);
void aud_set_audio_buffer_volume(aud_audio_buffer *b, float volume);
void aud_set_audio_buffer_pitch(aud_audio_buffer *b, float pitch);
void aud_track_audio_buffer(aud_audio_buffer *b);
void aud_untrack_audio_buffer(aud_audio_buffer *b);
unsigned int aud_read_audio_buffer_frames_in_internal_format(aud_audio_buffer *b, void *frames_out, unsigned int frame_count);
unsigned int aud_read_audio_buffer_frames_in_mixing_format(aud_audio_buffer *b, float *frames_out, unsigned int frame_count);
void aud_init_audio_buffer_pool(void);
void aud_close_audio_buffer_pool(void);

