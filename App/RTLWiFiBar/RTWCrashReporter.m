/*
 * RTWCrashReporter.m - self-diagnosing crash logging.
 *
 * A status-bar agent that dies silently after a reboot is undiagnosable:
 * no log line, no menu, just a missing icon. This module makes the app
 * write its own death certificate into the log the user already sends
 * back:
 *
 *   [CRASH] RTLWiFiBar fatal signal 11 (si_code 1, fault 0x...)
 *   [CRASH]   #00 0x... symbolName + offset (RTLWiFiBar)
 *   [CRASH]   #01 ...
 *
 * Design notes:
 *  - The SIGNAL path must not touch the Objective-C runtime, malloc-heavy
 *    APIs or the log lock (the crash may have happened while holding it).
 *    It writes raw bytes with open/write/close into a path captured at
 *    install time and formats with snprintf on the stack.
 *  - backtrace()/dladdr() are technically not async-signal-safe; they are
 *    the standard pragmatic trade-off for a lightweight reporter and are
 *    wrapped so any secondary failure just loses frames, never hangs.
 *  - After logging, every handler restores SIG_DFL and re-raises so the
 *    system crash reporter still runs and launchd still sees a failure.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#import "RTWCrashReporter.h"
#import "RTWLog.h"
#import <dlfcn.h>
#import <execinfo.h>
#import <fcntl.h>
#import <signal.h>
#import <stdio.h>
#import <string.h>
#import <sys/file.h>
#import <unistd.h>

static char gRTWCrashLogPath[1024] = {0};
static int  gRTWInstanceLockFD = -1;

/* ---------------------------------------------------------------- raw IO */

/* Append-only write used by signal handlers: no locks, no ObjC. */
static void RTWFatalWrite(const void *bytes, size_t length) {
    if (gRTWCrashLogPath[0] == '\0' || length == 0) return;
    int fd = open(gRTWCrashLogPath, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) return;
    ssize_t written = write(fd, bytes, length);
    (void)written;
    close(fd);
}

/* Best-effort frame dump in signal context. */
static void RTWPrintFrames(void) {
    void *frames[32];
    int count = backtrace(frames, 32);
    char line[300];

    int n = snprintf(line, sizeof(line),
                     "[CRASH]   backtrace: %d frames\n", count);
    if (n > 0) RTWFatalWrite(line, (size_t)n);

    for (int i = 0; i < count; ++i) {
        Dl_info info;
        if (dladdr(frames[i], &info) && info.dli_fname) {
            const char *base = strrchr(info.dli_fname, '/');
            base = base ? base + 1 : info.dli_fname;
            if (info.dli_sname) {
                ptrdiff_t offset = (char *)frames[i] - (char *)info.dli_saddr;
                n = snprintf(line, sizeof(line),
                             "[CRASH]   #%02d %p %s + %td (%s)\n",
                             i, frames[i], info.dli_sname, offset, base);
            } else {
                n = snprintf(line, sizeof(line),
                             "[CRASH]   #%02d %p (%s)\n", i, frames[i], base);
            }
        } else {
            n = snprintf(line, sizeof(line),
                         "[CRASH]   #%02d %p\n", i, frames[i]);
        }
        if (n > 0) RTWFatalWrite(line, (size_t)n);
    }
}

static void RTWRestoreDefaultAndReraise(int sig) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);
    raise(sig);
    /* If the re-raise is somehow swallowed, die the hard way anyway. */
    _exit(128 + sig);
}

/* --------------------------------------------------------------- handlers */

static void RTWFatalSignalHandler(int sig, siginfo_t *info, void *uap) {
    (void)uap;
    char line[200];
    int n = snprintf(line, sizeof(line),
                     "[CRASH] RTLWiFiBar fatal signal %d (si_code %d, fault %p)\n",
                     sig, info ? info->si_code : 0,
                     info ? info->si_addr : NULL);
    if (n > 0) RTWFatalWrite(line, (size_t)n);
    RTWPrintFrames();
    RTWRestoreDefaultAndReraise(sig);
}

static void RTWTerminateSignalHandler(int sig, siginfo_t *info, void *uap) {
    (void)info;
    (void)uap;
    char line[120];
    int n = snprintf(line, sizeof(line),
                     "[CRASH] RTLWiFiBar terminated by signal %d (external stop)\n",
                     sig);
    if (n > 0) RTWFatalWrite(line, (size_t)n);
    RTWRestoreDefaultAndReraise(sig);
}

static void RTWUncaughtExceptionHandler(NSException *exception) {
    /* NSExceptions arrive on a live, non-signal thread: the regular
     * RTWLog pipeline is safe here. */
    @try {
        RTWLog(@"[CRASH] uncaught exception %@ (%@)",
               exception.name, exception.reason);
        RTWLog(@"[CRASH] stack: %@",
               [exception.callStackSymbols componentsJoinedByString:@" | "]);
    } @catch (...) {
        /* Nothing left to lose - fall through to the abort below. */
    }
}

/* ---------------------------------------------------------------- install */

void RTWInstallCrashReporter(void) {
    NSString *logs = NSSearchPathForDirectoriesInDomains(
        NSLibraryDirectory, NSUserDomainMask, YES).firstObject;
    if (!logs.length) logs = NSHomeDirectory();
    NSString *path = [logs stringByAppendingPathComponent:@"Logs/RTLWiFiBar.log"];
    snprintf(gRTWCrashLogPath, sizeof(gRTWCrashLogPath), "%s",
             path.fileSystemRepresentation);

    NSSetUncaughtExceptionHandler(RTWUncaughtExceptionHandler);

    struct sigaction fatal;
    memset(&fatal, 0, sizeof(fatal));
    fatal.sa_sigaction = RTWFatalSignalHandler;
    fatal.sa_flags = SA_SIGINFO;
    sigemptyset(&fatal.sa_mask);
    sigaction(SIGSEGV, &fatal, NULL);
    sigaction(SIGABRT, &fatal, NULL);
    sigaction(SIGBUS, &fatal, NULL);
    sigaction(SIGILL, &fatal, NULL);
    sigaction(SIGFPE, &fatal, NULL);
    sigaction(SIGTRAP, &fatal, NULL);

    struct sigaction stop;
    memset(&stop, 0, sizeof(stop));
    stop.sa_sigaction = RTWTerminateSignalHandler;
    stop.sa_flags = SA_SIGINFO;
    sigemptyset(&stop.sa_mask);
    sigaction(SIGTERM, &stop, NULL);
    sigaction(SIGQUIT, &stop, NULL);
    sigaction(SIGHUP, &stop, NULL);
    sigaction(SIGINT, &stop, NULL);
}

/* ------------------------------------------------- single instance gate */

BOOL RTWAcquireSingleInstanceLock(void) {
    if (gRTWInstanceLockFD >= 0) return YES;

    NSString *dir = [NSHomeDirectory()
        stringByAppendingPathComponent:@"Library/Application Support"];
    [[NSFileManager defaultManager] createDirectoryAtPath:dir
                              withIntermediateDirectories:YES
                                               attributes:nil
                                                    error:nil];
    NSString *lockPath =
        [dir stringByAppendingPathComponent:@"RTLWiFiBar.instance.lock"];

    int fd = open(lockPath.fileSystemRepresentation, O_RDWR | O_CREAT, 0644);
    if (fd < 0) return YES; /* fail open: never block over a lock */
    if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        close(fd);
        RTWLog(@"second RTLWiFiBar instance detected (pid %d) - exiting",
               (int)getpid());
        return NO;
    }
    gRTWInstanceLockFD = fd; /* held for the process lifetime */
    return YES;
}
