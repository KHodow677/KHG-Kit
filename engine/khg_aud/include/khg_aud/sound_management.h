#include "khg_aud/utils.h"

void play_sound(sound s);
void stop_sound(sound s);
void pause_sound(sound s);
void resume_sound(sound s);
void play_sound_multi(sound s);
void stop_sound_multi(void);
int get_sounds_playing(void);
bool is_sound_playing(sound s);
void set_sound_volume(sound s, float volume);
void set_sound_pitch(sound s, float pitch);
void wave_format(wave *w, int sample_rate, int sample_size, int channels);
wave wave_copy(wave w);
void wave_crop(wave *w, int init_sample, int final_sample);
float *get_wave_data(wave w);
