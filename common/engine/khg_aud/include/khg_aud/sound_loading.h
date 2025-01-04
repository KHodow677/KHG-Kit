#include "khg_aud/utils.h"
#include <stdint.h>

bool aud_is_file_extension(const char *file_name, const char *ext);
unsigned char *aud_load_file_data(const char *fileName, unsigned int *bytesRead);
void aud_save_file_data(const char *file_name, void *data, unsigned int bytes_to_write);
void aud_save_file_text(const char *file_name, char *text);
aud_wave aud_load_WAV(const char *fileName);
aud_wave aud_load_OGG(const char *file_name);
aud_wave aud_load_FLAC(const char *file_name);
aud_wave aud_load_MP3(const char *file_name);
int aud_save_WAV(aud_wave w, const char *file_name);

aud_wave aud_load_wave(const char *file_name);
aud_sound aud_load_sound(const char *file_name);
aud_sound aud_load_sound_from_wave(aud_wave w);
void aud_update_sound(aud_sound s, const void *data, int samples_count);
void aud_unload_wave(aud_wave w);
void aud_unload_sound(aud_sound s);
void aud_export_wave(aud_wave w, const char *file_name);
void aud_export_wave_as_code(aud_wave w, const char *file_name);

