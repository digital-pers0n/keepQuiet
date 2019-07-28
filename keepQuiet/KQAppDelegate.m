//
//  KQAppDelegate.m
//  keepQuiet
//
//  Created by Terminator on 7/24/19.
//  Copyright © 2019 home. All rights reserved.
//

#import "KQAppDelegate.h"
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
        syslog(LOG_CRIT, "%s : InstallEventHandler() - %d", __PRETTY_FUNCTION__, error);
        exit(EXIT_FAILURE);
    }
    
    {   // Initialize hotkeys
    
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
            error = RegisterEventHotKey(
                                        ptr->key_code,
                                        ptr->modifier_mask,
                                        evenHotKeyID,
                                        GetEventDispatcherTarget(),
                                        0,
                                        _eventHotKeyRefs + i);
            if (error) {
                syslog(LOG_CRIT, "%s : RegisterEventHotKey() - %d", __PRETTY_FUNCTION__, error);
                exit(EXIT_FAILURE);
            }
        }
    }
}

- (void)applicationWillTerminate:(NSNotification *)notification {

}

#pragma mark - Hotkey callback

static OSStatus hotkey_callback(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData) {
    return noErr;
}

                                         
@end
