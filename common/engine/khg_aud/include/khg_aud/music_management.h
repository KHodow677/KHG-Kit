#include "khg_aud/utils.h"

aud_music aud_load_music_stream(const char *file_name);
void aud_unload_music_stream(aud_music m);
void aud_play_music_stream(aud_music m);
void aud_update_music_stream(aud_music m);
void aud_stop_music_stream(aud_music m);
void aud_pause_music_stream(aud_music m);
void aud_resume_music_stream(aud_music m);
bool aud_is_music_playing(aud_music m);
void aud_set_music_volume(aud_music m, float volume);
void aud_set_music_pitch(aud_music m, float pitch);
float aud_get_music_time_length(aud_music m);
float aud_get_music_time_played(aud_music m);

