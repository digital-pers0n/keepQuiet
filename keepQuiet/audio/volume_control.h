//
//  volume_control.h
//  keepQuiet
//
//  Created by Terminator on 7/24/19.
//  Copyright © 2019 home. All rights reserved.
//
//
// Based on ISSoundAdditions by Massimo Moiso (2012-09) InerziaSoft

#ifndef volume_control_h
#define volume_control_h

#import <CoreAudio/CoreAudio.h>
#import <AudioToolbox/AudioServices.h>

#define k_minimum_volume_level 0.005

/**
 
 @param device ID of the default audio device. This parameter cannot be NULL.
 
 @return @c noErr on success, if an error occurs the value referenced by @c device is left unchanged.
 */
OSStatus get_default_audio_device_id(AudioDeviceID *device);

/**
 Get the volume level of the default audio device.
 
 @param level The volume level of the default audio device. This parameter cannot be NULL.
 
 @return @c noErr on success, if an error occurs the value refrenced by @c level is left unchanged.
 */
OSStatus get_audio_volume(Float32 *level);

/**
 Set the volume level of the default audio device.
 
 @param level Valid range is between 0 and 1.
 
 @return @c noErr on success or an error code otherwise.
 */
OSStatus set_audio_volume(Float32 level);


/**
 Get the mute status of the default audio device.
 
 @param flag The mute status of the default device. @c true - the audio device is muted, @c false - the device is unmuted. This parameter cannot be NULL.
 
 @return @c noErr on success, if an error occurs the value refrenced by @c flag is left unchanged.
 */
OSStatus get_mute_status(Boolean *flag);


/**
 Set the mute status of the default audio device.
 
 @param flag Valid values are @c true to mute the audio device, @c false to unmute it.
 
 @return @c noErr on success or an error code otherwise.
 */
OSStatus set_mute_status(Boolean flag);

#endif /* volume_control_h */
