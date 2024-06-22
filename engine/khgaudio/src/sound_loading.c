#include "khgaudio/audio_buffer.h"
#include "khgaudio/sound_loading.h"
#include "khgaudio/utils.h"
#include "dr_libs/dr_flac.h"
#include "dr_libs/dr_mp3.h"
#include "dr_libs/dr_wav.h"
#include "khgutils/error_func.h"
#include "stb_vorbis/stb_vorbis.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool is_file_extension(const char *file_name, const char *ext) {
    bool result = false;
    const char *fileext;
    if ((fileext = strrchr(file_name, '.')) != NULL) {
        if (strcmp(fileext, ext) == 0) result = true;
    }
    return result;
}

unsigned char *load_file_data(const char *file_name, unsigned int *bytes_read) {
  unsigned char *data = NULL;
  *bytes_read = 0;
  if (file_name != NULL) {
    FILE *file = fopen(file_name, "rb");
    if (file != NULL) {
      fseek(file, 0, SEEK_END);
      int size = ftell(file);
      fseek(file, 0, SEEK_SET);
      if (size > 0) {
        data = (unsigned char *)malloc(size*sizeof(unsigned char));
        unsigned int count = (unsigned int)fread(data, sizeof(unsigned char), size, file);
        *bytes_read = count;
      }
      else {
        error_func("Failed to read file", user_defined_data);
      }
      fclose(file);
    }
    else {
      error_func("Failed to open file", user_defined_data);
    }
  }
  else {
    error_func("File name provided is not valid", user_defined_data);
  }
  return data;
}

void save_file_data(const char *file_name, void *data, unsigned int bytes_to_write) {
  if (file_name != NULL) {
    FILE *file = fopen(file_name, "wb");
    if (file != NULL) {
      unsigned int count = (unsigned int)fwrite(data, sizeof(unsigned char), bytes_to_write, file);
      if (count == 0) {
        error_func("Failed to write file", user_defined_data);
      }
      fclose(file);
    }
    else {
      error_func("Failed to open file", user_defined_data);
    }
  }
  else {
    error_func("File name provided is not valid", user_defined_data);
  }
}

void save_file_text(const char *file_name, char *text) {
  if (file_name != NULL) {
    FILE *file = fopen(file_name, "wt");
    if (file != NULL) {
      int count = fprintf(file, "%s", text);
      if (count == 0) {
        error_func("Failed to write text file", user_defined_data);
      }
      fclose(file);
    }
    else {
      error_func("Failed to open text file", user_defined_data);
    }
  }
  else {
    error_func("File name provided is not valid", user_defined_data);
  }
}

wave load_WAV(const char *file_name) {
  wave w = { 0 };
  unsigned int file_size = 0;
  unsigned char *file_data = load_file_data(file_name, &file_size);
  drwav wav = { 0 };
  bool success = drwav_init_memory(&wav, file_data, file_size, NULL);
  if (success) {
    w.sample_count = wav.totalPCMFrameCount * wav.channels;
    w.sample_rate = wav.sampleRate;
    w.sample_size = 16;
    w.channels = wav.channels;
    w.data = (short *)malloc(w.sample_count*sizeof(short));
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, w.data);
  }
  else {
    error_func("Failed to load WAV data", user_defined_data);
  }
  drwav_uninit(&wav);
  free(file_data);
  return w;
}

wave load_OGG(const char *file_name) {
  wave w = { 0 };
  unsigned int file_size = 0;
  unsigned char *file_data = load_file_data(file_name, &file_size);
  stb_vorbis *ogg_file = stb_vorbis_open_memory(file_data, file_size, NULL, NULL);
  if (ogg_file == NULL) {
    error_func("Failed to load OGG data", user_defined_data);
  }
  else {
    stb_vorbis_info info = stb_vorbis_get_info(ogg_file);
    w.sample_rate = info.sample_rate;
    w.sample_size = 16;
    w.channels = info.channels;
    w.sample_count = (unsigned int)stb_vorbis_stream_length_in_samples(ogg_file) * info.channels;
    float total_seconds = stb_vorbis_stream_length_in_seconds(ogg_file);
    w.data = (short *)malloc(w.sample_count * w.channels * sizeof(short));
    stb_vorbis_get_samples_short_interleaved(ogg_file, info.channels, (short *)w.data, w.sample_count * w.channels);
    stb_vorbis_close(ogg_file);
  }
  free(file_data);
  return w;
}

wave load_FLAC(const char *file_name) {
  wave w = { 0 };
  unsigned int file_size = 0;
  unsigned char *file_data = load_file_data(file_name, &file_size);
  unsigned long long int total_sample_count = 0;
  w.data = drflac_open_memory_and_read_pcm_frames_s16(file_data, file_size, &w.channels, &w.sample_rate, (drflac_uint64 *)&total_sample_count);
  if (w.data == NULL) {
    error_func("Failed to load FLAC data", user_defined_data);
  }
  else {
    w.sample_count = (unsigned int)total_sample_count;
    w.sample_size = 16;
  }
  free(file_data);
  return w;
}

wave load_MP3(const char *file_name) {
  wave w = { 0 };
  unsigned int file_size = 0;
  unsigned char *file_data = load_file_data(file_name, &file_size);
  unsigned long long int total_frame_count = 0;
  drmp3_config config = { 0 };
  w.data = drmp3_open_memory_and_read_f32(file_data, file_size, &config, (drmp3_uint64 *)&total_frame_count);
  if (w.data == NULL) {
    error_func("Failed to load MP3 data", user_defined_data);
  }
  else {
    w.channels = config.outputChannels;
    w.sample_rate = config.outputSampleRate;
    w.sample_count = (int)total_frame_count * w.channels;
    w.sample_size = 32;
  }
  free(file_data);
  return w;
}

int save_WAV(wave w, const char *file_name) {
  drwav wav = { 0 };
  drwav_data_format format = { 0 };
  format.container = drwav_container_riff;
  format.format = DR_WAVE_FORMAT_PCM;
  format.channels = w.channels;
  format.sampleRate = w.sample_rate;
  format.bitsPerSample = w.sample_size;
  drwav_init_file_write(&wav, file_name, &format, NULL);
  drwav_write_pcm_frames(&wav, w.sample_count/w.channels, w.data);
  drwav_uninit(&wav);
  return true;
}

wave load_wave(const char *file_name) {
  wave wave = { 0 };
  if (is_file_extension(file_name, ".wav")) {
    wave = load_WAV(file_name);
  }
  else if (is_file_extension(file_name, ".ogg")) {
    wave = load_OGG(file_name);
  }
  else if (is_file_extension(file_name, ".flac")) {
    wave = load_FLAC(file_name);
  }
  else if (is_file_extension(file_name, ".mp3")) {
    wave = load_MP3(file_name);
  }
  else {
    error_func("File format not supported", user_defined_data);
  }
  return wave;
}

sound load_sound(const char *file_name) {
  wave w = load_wave(file_name);
  sound s = load_sound_from_wave(w);
  unload_wave(w);
  return s;
}

sound load_sound_from_wave(wave w) {
  sound sound = { 0 };
  if (w.data != NULL) {
    ma_format format_in  = ((w.sample_size == 8)? ma_format_u8 : ((w.sample_size == 16)? ma_format_s16 : ma_format_f32));
    ma_uint32 frame_count_in = w.sample_count / w.channels;
    ma_uint32 frame_count = (ma_uint32)ma_convert_frames(NULL, 0, AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS, AUDIO_DEVICE_SAMPLE_RATE, NULL, frame_count_in, format_in, w.channels, w.sample_rate);
    if (frame_count == 0) {
      error_func("Failed to get frame count for format conversion", user_defined_data);
    }
    audio_buffer *audio_buffer = load_audio_buffer(AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS, AUDIO_DEVICE_SAMPLE_RATE, frame_count, AUDIO_BUFFER_USAGE_STATIC);
    if (audio_buffer == NULL) {
      error_func("Failed to create buffer", user_defined_data);
    }
    frame_count = (ma_uint32)ma_convert_frames(audio_buffer->data, frame_count, AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS, AUDIO_DEVICE_SAMPLE_RATE, w.data, frame_count_in, format_in, w.channels, w.sample_rate);
    if (frame_count == 0) {
      error_func("Failed format conversion", user_defined_data);
    }
    sound.sample_count = frame_count*AUDIO_DEVICE_CHANNELS;
    sound.stream.sample_rate = AUDIO_DEVICE_SAMPLE_RATE;
    sound.stream.sample_size = 32;
    sound.stream.channels = AUDIO_DEVICE_CHANNELS;
    sound.stream.buffer = audio_buffer;
  }
  return sound;
}

void update_sound(sound s, const void *data, int samples_count) {
  if (s.stream.buffer != NULL) {
    stop_audio_buffer(s.stream.buffer);
    memcpy(s.stream.buffer->data, data, samples_count * ma_get_bytes_per_frame(s.stream.buffer->converter.config.formatIn, s.stream.buffer->converter.config.channelsIn));
  }
}

void unload_wave(wave w) {
  if (w.data != NULL) {
    free(w.data);
  }
}

void unload_sound(sound s) {
  unload_audio_buffer(s.stream.buffer);
}

void export_wave(wave w, const char *file_name) {
  bool success = false;
  if (is_file_extension(file_name, ".wav")) {
    success = save_WAV(w, file_name);
  }
  if (is_file_extension(file_name, ".raw")) {
    save_file_data(file_name, w.data, w.sample_count * w.channels * w.sample_size / 8);
    success = true;
  }
  if (!success) {
    error_func("Failed to export wave data", user_defined_data);
  }
}

void export_wave_as_code(wave w, const char *file_name) {
  int text_bytes_per_line = 20;
  int wave_data_size = w.sample_count * w.channels * w.sample_size / 8;
  char *txt_data = (char *)calloc(6 * wave_data_size + 2000, sizeof(char));
  int bytes_count = 0;
  char var_file_name[256] = { 0 };
  strcpy(var_file_name, file_name);
  bytes_count += sprintf(txt_data + bytes_count, "// Wave data information\n");
  bytes_count += sprintf(txt_data + bytes_count, "#define %s_SAMPLE_COUNT     %u\n", var_file_name, w.sample_count);
  bytes_count += sprintf(txt_data + bytes_count, "#define %s_SAMPLE_RATE      %u\n", var_file_name, w.sample_rate);
  bytes_count += sprintf(txt_data + bytes_count, "#define %s_SAMPLE_SIZE      %u\n", var_file_name, w.sample_size);
  bytes_count += sprintf(txt_data + bytes_count, "#define %s_CHANNELS         %u\n\n", var_file_name, w.channels);
  bytes_count += sprintf(txt_data + bytes_count, "static unsigned char %s_DATA[%i] = { ", var_file_name, wave_data_size);
  for (int i = 0; i < wave_data_size - 1; i++) {
    bytes_count += sprintf(txt_data + bytes_count, ((i % text_bytes_per_line == 0)? "0x%x,\n" : "0x%x, "), ((unsigned char *)w.data)[i]);
  }
  bytes_count += sprintf(txt_data + bytes_count, "0x%x };\n", ((unsigned char *)w.data)[wave_data_size - 1]);
  save_file_text(file_name, txt_data);
  free(txt_data);
}
