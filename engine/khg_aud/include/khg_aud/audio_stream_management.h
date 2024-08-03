#include "khg_aud/utils.h"

aud_audio_stream aud_init_audio_stream(unsigned int sample_rate, unsigned int sample_size, unsigned int channels);
void aud_update_audio_stream(aud_audio_stream stream, const void *data, int samples_count);
void aud_close_audio_stream(aud_audio_stream stream);
bool aud_is_audio_stream_processed(aud_audio_stream stream);
void aud_play_audio_stream(aud_audio_stream stream);
void aud_pause_audio_stream(aud_audio_stream stream);
void aud_resume_audio_stream(aud_audio_stream stream);
bool aud_is_audio_stream_playing(aud_audio_stream stream);
void aud_stop_audio_stream(aud_audio_stream stream);
void aud_set_audio_stream_volume(aud_audio_stream stream, float volume);
void aud_set_audio_stream_pitch(aud_audio_stream stream, float pitch);
void aud_set_audiostream_buffer_size_default(int size);
