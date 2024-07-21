#include "khg_aud/utils.h"

bool is_file_extension(const char *file_name, const char *ext);
unsigned char *load_file_data(const char *fileName, unsigned int *bytesRead);
void save_file_data(const char *file_name, void *data, unsigned int bytes_to_write);
void save_file_text(const char *file_name, char *text);
wave load_WAV(const char *fileName);
wave load_OGG(const char *file_name);
wave load_FLAC(const char *file_name);
wave load_MP3(const char *file_name);
int save_WAV(wave w, const char *file_name);

wave load_wave(const char *file_name);
sound load_sound(const char *file_name);
sound load_sound_from_wave(wave w);
void update_sound(sound s, const void *data, int samples_count);
void unload_wave(wave w);
void unload_sound(sound s);
void export_wave(wave w, const char *file_name);
void export_wave_as_code(wave w, const char *file_name);
