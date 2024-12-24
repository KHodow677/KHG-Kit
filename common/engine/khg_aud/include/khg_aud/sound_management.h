#include "khg_aud/utils.h"

void aud_play_sound(aud_sound s);
void aud_stop_sound(aud_sound s);
void aud_pause_sound(aud_sound s);
void aud_resume_sound(aud_sound s);
void aud_play_sound_multi(aud_sound s);
void aud_stop_sound_multi(void);
int aud_get_sounds_playing(void);
bool aud_is_sound_playing(aud_sound s);
void aud_set_sound_volume(aud_sound s, float volume);
void aud_set_sound_pitch(aud_sound s, float pitch);
void aud_wave_format(aud_wave *w, int sample_rate, int sample_size, int channels);
aud_wave aud_wave_copy(aud_wave w);
void aud_wave_crop(aud_wave *w, int init_sample, int final_sample);
float *aud_get_wave_data(aud_wave w);

