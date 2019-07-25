//
//  volume_control.c
//  keepQuiet
//
//  Created by Terminator on 7/24/19.
//  Copyright © 2019 home. All rights reserved.
//
//
// Based on ISSoundAdditions by Massimo Moiso (2012-09) InerziaSoft

#include "volume_control.h"

#define err_fprintf(fmt, ...) fprintf(stderr, ("%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)

static inline OSStatus _get_default_audio_device_id(AudioDeviceID *device) {
    
    AudioObjectPropertyAddress property = {
        .mSelector = kAudioHardwarePropertyDefaultOutputDevice,
        .mScope = kAudioObjectPropertyScopeGlobal,
        .mElement = kAudioObjectPropertyElementMaster
    };
    
    OSStatus error = AudioObjectHasProperty(kAudioObjectSystemObject, &property);
    if (error != noErr) {
        err_fprintf("AudioObjectHasProperty() : %u\n", error);
        return error;
    }
    
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID result;
    error = AudioObjectGetPropertyData(
                                       kAudioObjectSystemObject,
                                       &property,
                                       0,
                                       NULL,
                                       &size,
                                       &result);
    if (error != noErr) {
        err_fprintf("AudioObjectGetPropertyData() : %u\n", error);
        return error;
    }
    
    *device = result;
    
    return noErr;
}


OSStatus get_default_audio_device_id(AudioDeviceID *device) {
    return _get_default_audio_device_id(device);
}


