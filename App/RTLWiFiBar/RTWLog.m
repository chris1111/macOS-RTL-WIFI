/*
 * RTWLog.m - plain-text log file backend (see RTWLog.h).
 *
 * Appends are done with POSIX open(O_APPEND)/write/close on purpose: the
 * macOS 26 SDK removed the classic NSFileHandle API (seekToFileEnd,
 * closeFile, ...), and the Swift-renamed variants (seekToEnd) never existed
 * as Objective-C selectors. C stdio/POSIX file calls are part of the OS ABI
 * and can never be deprecated out from under us.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#import "RTWLog.h"
#import <sys/stat.h>
#import <fcntl.h>
#import <unistd.h>

static NSString *RTWLogFilePath(void) {
    static NSString *path = nil;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        NSString *logs = NSSearchPathForDirectoriesInDomains(
            NSLibraryDirectory, NSUserDomainMask, YES).firstObject;
        if (!logs) logs = NSHomeDirectory();
        path = [logs stringByAppendingPathComponent:@"Logs/RTLWiFiBar.log"];
    });
    return path;
}

void RTWAppendLogFile(NSString *line) {
    @autoreleasepool {
        static NSLock *lock;
        static dispatch_once_t once;
        dispatch_once(&once, ^{
            lock = [[NSLock alloc] init];
        });
        [lock lock];

        NSString *path = RTWLogFilePath();
        NSFileManager *fm = [NSFileManager defaultManager];
        /* Rotate at 256 KB: keep exactly one previous generation. */
        NSDictionary *attrs = [fm attributesOfItemAtPath:path error:nil];
        if (attrs.fileSize > 256 * 1024) {
            [fm removeItemAtPath:[path stringByAppendingString:@".old"] error:nil];
            [fm moveItemAtPath:path toPath:[path stringByAppendingString:@".old"] error:nil];
        }
        [fm createDirectoryAtPath:path.stringByDeletingLastPathComponent
      withIntermediateDirectories:YES attributes:nil error:nil];

        NSDateFormatter *df = [[NSDateFormatter alloc] init];
        df.dateFormat = @"yyyy-MM-dd HH:mm:ss.SSS";
        NSString *stamped = [NSString stringWithFormat:@"[%@] %@\n",
                             [df stringFromDate:[NSDate date]], line];

        /* O_APPEND makes every write land at the current end; mode 0644 is
         * applied when the file is created. Best effort: a logging failure
         * must never take the app down. */
        const char *bytes = stamped.UTF8String;
        int fd = open(path.fileSystemRepresentation,
                      O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd >= 0) {
            NSUInteger len = [stamped lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
            ssize_t written = write(fd, bytes, len);
            (void)written;
            close(fd);
        }
        [lock unlock];
    }
}
