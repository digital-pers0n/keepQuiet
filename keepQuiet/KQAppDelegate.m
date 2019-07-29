//
//  KQAppDelegate.m
//  keepQuiet
//
//  Created by Terminator on 7/24/19.
//  Copyright © 2019 home. All rights reserved.
//

#import "KQAppDelegate.h"
#import "audio/volume_control.h"
#import <Carbon/Carbon.h>
#import <syslog.h>

static const OSType k_hotkey_signature = 'kqhk';

typedef NS_ENUM(UInt32, KQHotKeyID) {
    KQHotKeyVolumeUpPrecise,
    KQHotKeyVolumeDownPrecise,
    KQHotKeyVolumeUpNormal,
    KQHotKeyVolumeDownNormal,
    KQHotKeyVolumeMute,
};

typedef struct hotkey_data {
    KQHotKeyID key_id;
    UInt32 key_code;
    UInt32 modifier_mask;
} hotkey_init_data;

static const size_t k_number_of_hotkeys = KQHotKeyVolumeMute + 1;

@interface KQAppDelegate () {
    EventHotKeyRef _eventHotKeyRefs[k_number_of_hotkeys];
}

@end

@implementation KQAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    EventTypeSpec eventSpec = {
        .eventClass = kEventClassKeyboard,
        .eventKind = kEventHotKeyReleased,
    };
    OSStatus error = InstallEventHandler(
                                         GetApplicationEventTarget(),
                                         hotkey_callback,
                                         1,
                                         &eventSpec,
                                         (__bridge void *)self, NULL);
    
    if (error) {
        syslog(LOG_CRIT, "%s : InstallEventHandler() -> %d", __PRETTY_FUNCTION__, error);
        exit(EXIT_FAILURE);
    }
    
    if ([self initializeHotKeys] != noErr) {
        exit(EXIT_FAILURE);
    }
}

- (OSStatus)initializeHotKeys {
    
    hotkey_init_data hotkey_data[k_number_of_hotkeys] = {
        { .key_id = KQHotKeyVolumeUpPrecise, .key_code = kVK_F15, .modifier_mask = 0 },
        { .key_id = KQHotKeyVolumeDownPrecise, .key_code = kVK_F14, .modifier_mask = 0 },
        { .key_id = KQHotKeyVolumeUpNormal, .key_code = kVK_F15, .modifier_mask = shiftKey | cmdKey },
        { .key_id = KQHotKeyVolumeDownNormal, .key_code = kVK_F14, .modifier_mask = shiftKey | cmdKey },
        { .key_id = KQHotKeyVolumeMute, .key_code = kVK_F13, .modifier_mask = 0 },
    };
    
    EventHotKeyID evenHotKeyID = { .signature = k_hotkey_signature };
    for (int i = 0; i < k_number_of_hotkeys; i++) {
        hotkey_init_data *ptr = hotkey_data + i;
        evenHotKeyID.id = ptr->key_id;
        OSStatus error = RegisterEventHotKey(
                                    ptr->key_code,
                                    ptr->modifier_mask,
                                    evenHotKeyID,
                                    GetEventDispatcherTarget(),
                                    0,
                                    _eventHotKeyRefs + i);
        if (error) {
            syslog(LOG_CRIT, "%s : RegisterEventHotKey() -> %d", __PRETTY_FUNCTION__, error);
            return error;
        }
    }
    
    return noErr;
}

- (void)applicationWillTerminate:(NSNotification *)notification {

}

#pragma mark - Hotkey callback

static inline OSStatus _adjust_audio_volume(Float32 value) {
    Float32 current_volume;
    OSStatus error = get_audio_volume(&current_volume);
    
    if (error) {
        syslog(LOG_WARNING, "get_audio_volume() -> %d", error);
        return error;
    }
    
    error = set_audio_volume(current_volume + value);
    
    if (error) {
        syslog(LOG_WARNING, "set_audio_volume() -> %d", error);
        return error;
    }
    
    NSBeep();
    return noErr;
}

static inline OSStatus _toggle_mute() {
    Boolean flag;
    OSStatus error = get_mute_status(&flag);
    
    if (error) {
        syslog(LOG_WARNING, "get_mute_status() -> %d", error);
        return error;
    }
    
    error = set_mute_status((flag) ? false : true);
    
    if (error) {
        syslog(LOG_WARNING, "set_mute_status() -> %d", error);
        return error;
    }
    
    return noErr;
}

static OSStatus hotkey_callback(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData) {
    
    EventHotKeyID hotKeyID;
    OSStatus error = GetEventParameter(theEvent, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(hotKeyID), NULL, &hotKeyID);
    
    if (error != noErr) {
        syslog(LOG_WARNING, "GetEventParameter() -> %d", error);
        return error;
    }
    
    if (hotKeyID.signature == k_hotkey_signature) {

        switch (hotKeyID.id) {
            case KQHotKeyVolumeUpPrecise:
                return _adjust_audio_volume(0.01);
                
                break;
                
            case KQHotKeyVolumeDownPrecise:
                return _adjust_audio_volume(-0.01);
                
                break;
                
            case KQHotKeyVolumeUpNormal:
                return _adjust_audio_volume(0.1);
                
                break;
                
            case KQHotKeyVolumeDownNormal:
                return _adjust_audio_volume(-0.1);
                
                break;
                
            case KQHotKeyVolumeMute:
                return _toggle_mute();

                break;
                
            default:
                break;
        }
    }
    
    return noErr;
}

                                         
@end
