#define DR_FLAC_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define JAR_MOD_IMPLEMENTATION
#define JAR_XM_IMPLEMENTATION

#include "khg_aud/audio_stream_management.h"
#include "khg_aud/music_management.h"
#include "khg_aud/sound_loading.h"
#include "khg_utl/error_func.h"
#include "dr_libs/dr_flac.h"
#include "dr_libs/dr_mp3.h"
#include "dr_libs/dr_wav.h"
#include "jar_libs/jar_mod.h"
#include "jar_libs/jar_xm.h"
#include "stb_vorbis/stb_vorbis.h"
#include <stdlib.h>

aud_music aud_load_music_stream(const char *file_name) {
  aud_music m = { 0 };
  bool music_loaded = false;
  if (aud_is_file_extension(file_name, ".wav")) {
    drwav *ctx_wav = malloc(sizeof(drwav));
    bool success = drwav_init_file(ctx_wav, file_name, NULL);
    if (success) {
        m.ctx_type = AUD_MUSIC_AUDIO_WAV;
        m.ctx_data = ctx_wav;
        m.stream = aud_init_audio_stream(ctx_wav->sampleRate, ctx_wav->bitsPerSample, ctx_wav->channels);
        m.sample_count = (unsigned int)ctx_wav->totalPCMFrameCount*ctx_wav->channels;
        m.looping = true;
        music_loaded = true;
    }
  }
  else if (aud_is_file_extension(file_name, ".ogg")) {
    m.ctx_data = stb_vorbis_open_filename(file_name, NULL, NULL);
    if (m.ctx_data != NULL) {
      m.ctx_type = AUD_MUSIC_AUDIO_OGG;
      stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis *)m.ctx_data);
      m.stream = aud_init_audio_stream(info.sample_rate, 16, info.channels);
      m.sample_count = (unsigned int)stb_vorbis_stream_length_in_samples((stb_vorbis *)m.ctx_data) * info.channels;
      m.looping = true;
      music_loaded = true;
    }
  }
  else if (aud_is_file_extension(file_name, ".flac")) {
    m.ctx_data = drflac_open_file(file_name);
    if (m.ctx_data != NULL) {
      m.ctx_type = AUD_MUSIC_AUDIO_FLAC;
      drflac *ctx_flac = (drflac *)m.ctx_data;
      m.stream = aud_init_audio_stream(ctx_flac->sampleRate, ctx_flac->bitsPerSample, ctx_flac->channels);
      m.sample_count = (unsigned int)ctx_flac->totalSampleCount;
      m.looping = true;
      music_loaded = true;
    }
  }
  else if (aud_is_file_extension(file_name, ".mp3")) {
    drmp3 *ctx_mp3 = malloc(sizeof(drmp3));
    m.ctx_data = ctx_mp3;
    int result = drmp3_init_file(ctx_mp3, file_name, NULL);
    if (result > 0) {
        m.ctx_type = AUD_MUSIC_AUDIO_MP3;
        m.stream = aud_init_audio_stream(ctx_mp3->sampleRate, 32, ctx_mp3->channels);
        m.sample_count = (unsigned int)drmp3_get_pcm_frame_count(ctx_mp3) * ctx_mp3->channels;
        m.looping = true;
        music_loaded = true;
    }
  }
  else if (aud_is_file_extension(file_name, ".xm")) {
    jar_xm_context_t *ctx_xm = NULL;
    int result = jar_xm_create_context_from_file(&ctx_xm, 48000, file_name);
    if (result == 0) {
      m.ctx_type = AUD_MUSIC_MODULE_XM;
      jar_xm_set_max_loop_count(ctx_xm, 0);
      m.stream = aud_init_audio_stream(48000, 16, 2);
      m.sample_count = (unsigned int)jar_xm_get_remaining_samples(ctx_xm) * 2;
      m.looping = true;
      jar_xm_reset(ctx_xm);
      music_loaded = true;
      m.ctx_data = ctx_xm;
    }
  }
  else if (aud_is_file_extension(file_name, ".mod")) {
    jar_mod_context_t *ctx_mod = malloc(sizeof(jar_mod_context_t));
    jar_mod_init(ctx_mod);
    int result = jar_mod_load_file(ctx_mod, file_name);
    if (result > 0) {
        m.ctx_type = AUD_MUSIC_MODULE_MOD;
        m.stream = aud_init_audio_stream(48000, 16, 2);
        m.sample_count = (unsigned int)jar_mod_max_samples(ctx_mod)*2;
        m.looping = true;
        music_loaded = true;
        m.ctx_data = ctx_mod;
    }
  }
  else {
    utl_error_func("File format not supported", utl_user_defined_data);
  }
  if (!music_loaded) {
    if (m.ctx_type == AUD_MUSIC_AUDIO_WAV) {
      drwav_uninit((drwav *)m.ctx_data);
    }
    else if (m.ctx_type == AUD_MUSIC_AUDIO_OGG) {
      stb_vorbis_close((stb_vorbis *)m.ctx_data);
    }
    else if (m.ctx_type == AUD_MUSIC_AUDIO_FLAC) {
      drflac_free((drflac *)m.ctx_data);
    }
    else if (m.ctx_type == AUD_MUSIC_AUDIO_MP3) {
      drmp3_uninit((drmp3 *)m.ctx_data); 
      free(m.ctx_data);
    }
    else if (m.ctx_type == AUD_MUSIC_MODULE_XM) {
      jar_xm_free_context((jar_xm_context_t *)m.ctx_data);
    }
    else if (m.ctx_type == AUD_MUSIC_MODULE_MOD) {
      jar_mod_unload((jar_mod_context_t *)m.ctx_data); 
      free(m.ctx_data);
    }
    utl_error_func("Music file could not be opened", utl_user_defined_data);
  }
  return m;
}

void aud_unload_music_stream(aud_music m) {
  aud_close_audio_stream(m.stream);
  if (m.ctx_type == AUD_MUSIC_AUDIO_WAV) {
    drwav_uninit((drwav *)m.ctx_data);
  }
  else if (m.ctx_type == AUD_MUSIC_AUDIO_OGG) {
    stb_vorbis_close((stb_vorbis *)m.ctx_data);
  }
  else if (m.ctx_type == AUD_MUSIC_AUDIO_FLAC) {
    drflac_free((drflac *)m.ctx_data);
  }
  else if (m.ctx_type == AUD_MUSIC_AUDIO_MP3) {
    drmp3_uninit((drmp3 *)m.ctx_data); 
    free(m.ctx_data);
  }
  else if (m.ctx_type == AUD_MUSIC_MODULE_XM) {
    jar_xm_free_context((jar_xm_context_t *)m.ctx_data);
  }
  else if (m.ctx_type == AUD_MUSIC_MODULE_MOD) {
    jar_mod_unload((jar_mod_context_t *)m.ctx_data); 
    free(m.ctx_data);
  }
}

void aud_play_music_stream(aud_music m) {
  if (m.stream.buffer != NULL) {
    unsigned int frame_cursor_pos = m.stream.buffer->frame_cursor_pos;
    aud_play_audio_stream(m.stream);
    m.stream.buffer->frame_cursor_pos = frame_cursor_pos;
  }
}

void aud_update_music_stream(aud_music m) {
  if (m.stream.buffer == NULL) {
    return;
  }
  bool streamEnding = false;
  unsigned int subBufferSizeInFrames = m.stream.buffer->size_in_frames / 2;
  void *pcm = calloc(subBufferSizeInFrames*m.stream.channels * m.stream.sample_size / 8, 1);
  int samplesCount = 0;
  int sampleLeft = m.sample_count - (m.stream.buffer->total_frames_processed * m.stream.channels);
  while (aud_is_audio_stream_processed(m.stream)) {
    if ((sampleLeft / m.stream.channels) >= subBufferSizeInFrames) {
      samplesCount = subBufferSizeInFrames * m.stream.channels;
    }
    else {
      samplesCount = sampleLeft;
    }
    switch (m.ctx_type) {
      case AUD_MUSIC_AUDIO_WAV:
        drwav_read_pcm_frames_s16((drwav *)m.ctx_data, samplesCount / m.stream.channels, (short *)pcm);
        break;
      case AUD_MUSIC_AUDIO_OGG:
        stb_vorbis_get_samples_short_interleaved((stb_vorbis *)m.ctx_data, m.stream.channels, (short *)pcm, samplesCount);
        break;
      case AUD_MUSIC_AUDIO_FLAC:
        drflac_read_pcm_frames_s16((drflac *)m.ctx_data, samplesCount, (short *)pcm);
        break;
      case AUD_MUSIC_AUDIO_MP3:
        drmp3_read_pcm_frames_f32((drmp3 *)m.ctx_data, samplesCount / m.stream.channels, (float *)pcm);
        break;
      case AUD_MUSIC_MODULE_XM:
        jar_xm_generate_samples_16bit((jar_xm_context_t *)m.ctx_data, (short *)pcm, samplesCount / 2);
        break;
      case AUD_MUSIC_MODULE_MOD:
        jar_mod_fillbuffer((jar_mod_context_t *)m.ctx_data, (short *)pcm, samplesCount / 2, 0);
        break;
      default:
        break;
    }
    aud_update_audio_stream(m.stream, pcm, samplesCount);
    if ((m.ctx_type == AUD_MUSIC_MODULE_XM) || (m.ctx_type == AUD_MUSIC_MODULE_MOD)) {
      if (samplesCount > 1) {
        sampleLeft -= samplesCount / 2;
      }
      else {
        sampleLeft -= samplesCount;
      }
    }
    else {
      sampleLeft -= samplesCount;
    }
    if (sampleLeft <= 0) {
      streamEnding = true;
      break;
    }  
  }
  free(pcm);
  if (streamEnding) {
    aud_stop_music_stream(m);
    if (m.looping) {
      aud_play_music_stream(m);
    }
  }
  else {
    if (aud_is_music_playing(m)) {
      aud_play_music_stream(m);
    }
  }
}

void aud_stop_music_stream(aud_music m) {
  aud_stop_audio_stream(m.stream);
  switch (m.ctx_type) {
    case AUD_MUSIC_AUDIO_WAV:
      drwav_seek_to_pcm_frame((drwav *)m.ctx_data, 0); 
      break;
    case AUD_MUSIC_AUDIO_OGG:
      stb_vorbis_seek_start((stb_vorbis *)m.ctx_data); 
      break;
    case AUD_MUSIC_AUDIO_FLAC:
      drflac_seek_to_pcm_frame((drflac *)m.ctx_data, 0); 
      break;
    case AUD_MUSIC_AUDIO_MP3:
      drmp3_seek_to_pcm_frame((drmp3 *)m.ctx_data, 0); 
      break;
    case AUD_MUSIC_MODULE_XM:
      jar_xm_reset((jar_xm_context_t *)m.ctx_data);
      break;
    case AUD_MUSIC_MODULE_MOD: 
      jar_mod_seek_start((jar_mod_context_t *)m.ctx_data);
      break;
    default:
      break;
  }
}

void aud_pause_music_stream(aud_music m) {
  aud_pause_audio_stream(m.stream);
}

void aud_resume_music_stream(aud_music m) {
  aud_resume_audio_stream(m.stream);
}

bool aud_is_music_playing(aud_music m) {
  return aud_is_audio_stream_playing(m.stream);
}

void aud_set_music_volume(aud_music m, float volume) {
  aud_set_audio_stream_volume(m.stream, volume);
}

void aud_set_music_pitch(aud_music m, float pitch) {
  aud_set_audio_stream_pitch(m.stream, pitch);
}

float aud_get_music_time_length(aud_music m) {
  float totalSeconds = 0.0f;
  totalSeconds = (float)m.sample_count / (m.stream.sample_rate * m.stream.channels);
  return totalSeconds;
}

float aud_get_music_time_played(aud_music m) {
  float secondsPlayed = 0.0f;
  if (m.stream.buffer != NULL) {
    unsigned int samplesPlayed = m.stream.buffer->total_frames_processed * m.stream.channels;
    secondsPlayed = (float)samplesPlayed / (m.stream.sample_rate * m.stream.channels);
  }
  return secondsPlayed;
}
