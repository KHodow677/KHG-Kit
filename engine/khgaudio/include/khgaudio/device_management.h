#pragma once

#ifndef __cplusplus
  #if !defined(_STDBOOL_H)
    typedef enum { false, true } bool;
    #define _STDBOOL_H
  #endif
#endif

void init_audio_device(void);
void close_audio_device(void);
bool is_audio_device_ready(void);
void set_master_volume(float volume);

