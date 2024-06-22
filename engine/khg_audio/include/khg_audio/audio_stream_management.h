#include "khg_audio/utils.h"

audio_stream init_audio_stream(unsigned int sample_rate, unsigned int sample_size, unsigned int channels);
void update_audio_stream(audio_stream stream, const void *data, int samples_count);
void close_audio_stream(audio_stream stream);
bool is_audio_stream_processed(audio_stream stream);
void play_audio_stream(audio_stream stream);
void pause_audio_stream(audio_stream stream);
void resume_audio_stream(audio_stream stream);
bool is_audio_stream_playing(audio_stream stream);
void stop_audio_stream(audio_stream stream);
void set_audio_stream_volume(audio_stream stream, float volume);
void set_audio_stream_pitch(audio_stream stream, float pitch);
void set_audiostream_buffer_size_default(int size);
