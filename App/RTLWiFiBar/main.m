/*
 * main.m - RTLWiFiBar entry point.
 *
 * The app is LSUIElement: it lives only in the menu bar, has no Dock
 * presence and never becomes the active app except for its own dialogs.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#import <AppKit/AppKit.h>
#import "AppDelegate.h"
#import "RTWCrashReporter.h"

int main(int argc, const char *argv[]) {
    (void)argc; (void)argv;
    /* Crash logging + single-instance gate go in before anything else: a
     * status-bar agent that dies silently after reboot is undiagnosable. */
    RTWInstallCrashReporter();
    if (!RTWAcquireSingleInstanceLock()) return 0;
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        /* Belt and braces next to LSUIElement for launchd starts. */
        if ([app respondsToSelector:@selector(setActivationPolicy:)])
            [app setActivationPolicy:NSApplicationActivationPolicyAccessory];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        app.delegate = delegate;
        [app run];
    }
    return 0;
}
