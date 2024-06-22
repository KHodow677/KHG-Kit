#include "khg_audio/utils.h"

music load_music_stream(const char *file_name);
void unload_music_stream(music m);
void play_music_stream(music m);
void update_music_stream(music m);
void stop_music_stream(music m);
void pause_music_stream(music m);
void resume_music_stream(music m);
bool is_music_playing(music m);
void set_music_volume(music m, float volume);
void set_music_pitch(music m, float pitch);
float get_music_time_length(music m);
float get_music_time_played(music m);
