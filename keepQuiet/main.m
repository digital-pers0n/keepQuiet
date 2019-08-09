//
//  main.m
//  keepQuiet
//
//  Created by Terminator on 7/24/19.
//  Copyright © 2019 home. All rights reserved.
//

#import "KQAppDelegate.h"
#import "command_line.h"

int main(int argc, const char * argv[]) {
    
    if (argc > 1) {
        return read_options(argc, (char **)argv);
    }

    /* Global hot keys only work in accordance with Cocoa/Carbon applications */
    NSApplication *application = NSApplication.new;
    KQAppDelegate *delegate = KQAppDelegate.new;
    application.delegate = delegate;
    [application run];
    
    return 0;
}
