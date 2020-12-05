//
//  command_line.c
//  keepQuiet
//
//  Created by Terminator on 8/7/19.
//  Copyright © 2019 home. All rights reserved.
//

#include "command_line.h"
#include "volume_control.h"
#include <getopt.h>

typedef struct cmd_flags_ {
    unsigned int get_volume:1;
    unsigned int set_volume:1;
    unsigned int adjust_volume:1;
    unsigned int toggle_mute:1;
    unsigned int print_help:1;
    unsigned int print_version:1;
} cmd_flags;

typedef struct cmd_data_ {
    cmd_flags flags;
    float new_volume_level;
    float adjust_amount;
} cmd_data;

static void _print_version() {
    printf("%s %s, %s\n", PROGRAM_NAME, PROGRAM_VERSION, __DATE__);
}

static void _print_help() {
    _print_version();
    printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
    printf("       Launch without options to run %s in the background.\n\n", PROGRAM_NAME);
    puts("  -g --get_volume             get audio volume");
    puts("  -s --set_volume <0..1>      set audio volume");
    puts("  -a --adjust_volume <-1..1>  adjust audio volume");
    puts("  -m --mute                   toggle mute");
    puts("  -v --version                print programm version");
    puts("  -h --help                   print this message\n");
}

static int _read_options(int argc, char *argv[], cmd_data *out_data) {
    const char *short_options = "gs:a:mdhv";
    struct option long_options[] = {
        { .name = "get_volume",    .has_arg = no_argument,       .flag = NULL, .val = 'g' },
        { .name = "set_volume",    .has_arg = required_argument, .flag = NULL, .val = 's' },
        { .name = "adjust_volume", .has_arg = required_argument, .flag = NULL, .val = 'a' },
        { .name = "mute",          .has_arg = no_argument,       .flag = NULL, .val = 'm' },
        { .name = "help",          .has_arg = no_argument,       .flag = NULL, .val = 'h' },
        { .name = "version",       .has_arg = no_argument,       .flag = NULL, .val = 'v' },
        { 0 }
    };
    
    int ch;
    
    while ((ch = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (ch) {
            case 'g':
                out_data->flags.get_volume = 1;
                break;
                
            case 's':
                out_data->flags.set_volume = 1;
                out_data->new_volume_level = strtof(optarg, NULL);
                break;
                
            case 'a':
                out_data->flags.adjust_volume = 1;
                out_data->adjust_amount = strtof(optarg, NULL);
                
            case 'm':
                out_data->flags.toggle_mute = 1;
                break;
                
            case 'h':
                out_data->flags.print_help = 1;
                break;
                
            case 'v':
                out_data->flags.print_version = 1;
                break;
                
            case '?':
                fprintf(stderr, "Run '%s -h' for more info.\n", PROGRAM_NAME);
                return EXIT_FAILURE;
                break;
                
            default:
                break;
        }
    }
    
    return 0;
}

static OSStatus _toggle_mute() {
    Boolean flag;
    OSStatus error = get_mute_status(&flag);
    
    if (error) {
        return error;
    }
    
    return set_mute_status((flag) ? false : true);
}

static OSStatus _adjust_audio_volume(Float32 amount) {
    Float32 current_volume;
    OSStatus error = get_audio_volume(&current_volume);
    
    if (error) {
        return error;
    }
    
    return set_audio_volume(current_volume + amount);
}

int read_options(int argc, char *argv[]) {
    cmd_data data = { 0 };
    int result = _read_options(argc, argv, &data);
    
    if (result != 0) {
        return EXIT_FAILURE;
    }
    
    if (data.flags.print_help) {
        _print_help();
        return 0;
    }
    
    if (data.flags.print_version) {
        _print_version();
        return 0;
    }
    
    if (data.flags.toggle_mute) {
        if (_toggle_mute() != noErr) {
            return EXIT_FAILURE;
        }
    }
    
    if (data.flags.get_volume) {
        Float32 result;
        if (get_audio_volume(&result) != noErr) {
            return EXIT_FAILURE;
        }
        printf("%g\n", result);
    }
    
    if (data.flags.set_volume) {
        Float32 value = data.new_volume_level;
        if (set_audio_volume(value) != noErr) {
            return EXIT_FAILURE;
        }
    }
    
    if (data.flags.adjust_volume) {
        Float32 value = data.adjust_amount;
        if (_adjust_audio_volume(value) != noErr) {
            return EXIT_FAILURE;
        }
    }

    return 0;
}
