#pragma once

#ifndef __cplusplus
// Boolean type
    #if !defined(_STDBOOL_H)
        typedef enum { false, true } bool;
        #define _STDBOOL_H
    #endif
#endif

void initAudioDevice(void);
void closeAudioDevice(void);
void isAudioDeviceReady(void);
void setMasterVolume(float volume);

