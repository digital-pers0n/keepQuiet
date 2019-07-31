//
//  main.m
//  keepQuiet
//
//  Created by Terminator on 7/24/19.
//  Copyright © 2019 home. All rights reserved.
//

#import "KQAppDelegate.h"

#define PROGRAM_NAME "keepQuiet"
#define PROGRAM_VERSION "0.1"

int main(int argc, const char * argv[]) {
    if (argc > 1) {
        const char *arg  = argv[1];
        if (strncmp(arg, "-v", 2) == 0) {
            printf("%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);
            return 0;
        };
    }
    /* Global hot keys only work in accordance with Cocoa/Carbon applications */
    NSApplication *application = NSApplication.new;
    KQAppDelegate *delegate = KQAppDelegate.new;
    application.delegate = delegate;
    [application run];
    
    return 0;
}
