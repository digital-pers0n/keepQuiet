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
    
    if (!AudioObjectHasProperty(kAudioObjectSystemObject, &property)) {
        err_fprintf("AudioObjectHasProperty()\n");
        return -1;
    }
    
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID result;
    OSStatus error = AudioObjectGetPropertyData(
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

static inline Float32 _normalize_volume(Float32 value) {
    if (value > 1.0) {
        return 1.0;
    } else if (value < 0.0) {
        return 0.0;
    }
    return value;
}

static inline OSStatus _get_audio_volume(Float32 *volume_level) {
    
    AudioDeviceID device;
    OSStatus error = _get_default_audio_device_id(&device);
    if (error != noErr) {
        return error;
    }
    
    AudioObjectPropertyAddress property = {
        .mSelector = kAudioHardwareServiceDeviceProperty_VirtualMasterVolume,
        .mScope = kAudioDevicePropertyScopeOutput,
        .mElement = kAudioObjectPropertyElementMaster
    };
    
    if (!AudioObjectHasProperty(device, &property)) {
        err_fprintf("AudioObjectHasProperty()\n");
        return -1;
    }
    
    UInt32 size = sizeof(Float32);
    Float32 result;
    error = AudioObjectGetPropertyData(device, &property, 0, NULL, &size, &result);
    if (error != noErr) {
        err_fprintf("AudioObjectGetPropertyData() : %u\n", error);
        return error;
    }
    
    result = _normalize_volume(result);
    *volume_level = result;
    
    return error;
}

OSStatus get_audio_volume(Float32 *level) {
    return _get_audio_volume(level);
}

OSStatus _set_audio_volume(Float32 volume_level) {
    
    AudioDeviceID device;
    OSStatus error = _get_default_audio_device_id(&device);
    if (error != noErr) {
        return error;
    }
    
    volume_level = _normalize_volume(volume_level);
    
    AudioObjectPropertyAddress property = {
        .mElement = kAudioObjectPropertyElementMaster,
        .mScope = kAudioDevicePropertyScopeOutput
    };
    
    Boolean has_mute_property = false, can_set_mute_property = false;
    Boolean should_mute = (volume_level < k_minimum_volume_level);
    if (should_mute) {
        property.mSelector = kAudioDevicePropertyMute;
        has_mute_property = AudioObjectHasProperty(device, &property);
        if (has_mute_property) {
            AudioObjectIsPropertySettable(device, &property, &can_set_mute_property);
        }
    } else {
        property.mSelector = kAudioHardwareServiceDeviceProperty_VirtualMasterVolume;
    }
    
    if (!AudioObjectHasProperty(device, &property)) {
        err_fprintf("AudioObjectHasProperty()");
        return -1;
    }
    
    Boolean can_set_volume_property = false;
    error = AudioObjectIsPropertySettable(device, &property, &can_set_volume_property);
    if (error != noErr) {
        err_fprintf("AudioObjectIsPropertySettable() : %u\n", error);
        return error;
    }
    if (!can_set_volume_property) {
        err_fprintf("Cannot set volume\n");
        return -1;
    }
    
    if (should_mute && has_mute_property && can_set_mute_property) {
        UInt32 mute = 1;
        error = AudioObjectSetPropertyData(device, &property, 0, NULL, sizeof(mute), &mute);
        if (error != noErr) {
            err_fprintf("AudioObjectSetPropertyData() : %u\n", error);
            return error;
        }
    } else {
        error = AudioObjectSetPropertyData(device, &property, 0, NULL, sizeof(volume_level), &volume_level);
        if (error != noErr) {
            err_fprintf("AudioObjectSetPropertyData() : %u\n", error);
            return error;
        }
        
        if (has_mute_property && can_set_mute_property) {
            property.mSelector = kAudioDevicePropertyMute;
            UInt32 mute = 0;
            error = AudioObjectSetPropertyData(device, &property, 0, NULL, sizeof(mute), &mute);
            if (error != noErr) {
                err_fprintf("AudioObjectSetPropertyData() : %u\n", error);
                return error;
            }
        }
    }
    
    return error;
}

OSStatus set_audio_volume(Float32 level) {
    return _set_audio_volume(level);
}

OSStatus _get_mute_status(Boolean *flag) {
    
    AudioDeviceID device;
    OSStatus error = _get_default_audio_device_id(&device);
    if (error != noErr) {
        return error;
    }
    
    AudioObjectPropertyAddress property = {
        .mElement = kAudioObjectPropertyElementMaster,
        .mScope = kAudioDevicePropertyScopeOutput,
        .mSelector = kAudioDevicePropertyMute
    };
    
    Boolean has_mute_property = AudioObjectHasProperty(device, &property);
    if (has_mute_property) {
        UInt32 result = 0;
        UInt32 size = 0;
        error = AudioObjectGetPropertyData(device, &property, 0, NULL, &size, &result);
        if (error != noErr) {
            err_fprintf("AudioObjectGetPropertyData() : %u\n", error);
            return error;
        }
        *flag = result;
    }
    return error;
}

OSStatus get_mute_status(Boolean *flag) {
    return _get_mute_status(flag);
}

OSStatus _set_mute_status(Boolean flag) {
    AudioDeviceID device;
    OSStatus error = _get_default_audio_device_id(&device);
    if (error != noErr) {
        return error;
    }
    
    AudioObjectPropertyAddress property = {
        .mElement = kAudioObjectPropertyElementMaster,
        .mScope = kAudioDevicePropertyScopeOutput,
        .mSelector = kAudioDevicePropertyMute
    };
    
    Boolean has_mute_property = AudioObjectHasProperty(device, &property);
    Boolean can_set_mute_property = false;
    
    if (has_mute_property) {
        OSStatus error = AudioObjectIsPropertySettable(device, &property, &can_set_mute_property);
        if (error != noErr) {
            err_fprintf("AudioObjectIsPropertySettable() : %u\n", error);
            return error;
        }
        if (!can_set_mute_property) {
            err_fprintf("Cannot set mute property\n");
            return -1;
        }
        UInt32 value = flag;
        error = AudioObjectSetPropertyData(device, &property, 0, NULL, sizeof(value), &value);
        if (error != noErr) {
            err_fprintf("AudioObjectSetPropertyData() : %u\n", error);
            return error;
        }
    }
    return error;
}

OSStatus set_mute_status(Boolean flag) {
    return _set_mute_status(flag);
}
