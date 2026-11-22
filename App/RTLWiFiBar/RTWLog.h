/*
 * RTWLog.h - tiny shared logging for RTLWiFiBar.
 *
 * Everything interesting goes to two places:
 *   1. NSLog -> the unified log (Console.app, filter "RTLWiFiBar")
 *   2. ~/Library/Logs/RTLWiFiBar.log  (plain text, easy to paste back)
 *
 * RTWAppendLogFile() is safe to call from any thread; it opens, appends and
 * closes per line and rotates the file at 256 KB so it can never grow away.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#import <Foundation/Foundation.h>

#define RTWLog(...) do { \
    NSLog(__VA_ARGS__); \
    NSString *_rtwLine = [[NSString alloc] initWithFormat:__VA_ARGS__]; \
    RTWAppendLogFile(_rtwLine); \
} while (0)

void RTWAppendLogFile(NSString *line);
