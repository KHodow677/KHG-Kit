#pragma once

#include "khg_aud/utils.h"
#include "miniaudio/miniaudio.h"

aud_audio_buffer *aud_load_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 size_in_frames, int usage);
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
ma_uint32 aud_read_audio_buffer_frames_in_internal_format(aud_audio_buffer *b, void *frames_out, ma_uint32 frame_count);
ma_uint32 aud_read_audio_buffer_frames_in_mixing_format(aud_audio_buffer *b, float *frames_out, ma_uint32 frame_count);
void aud_init_audio_buffer_pool(void);
void aud_close_audio_buffer_pool(void);

