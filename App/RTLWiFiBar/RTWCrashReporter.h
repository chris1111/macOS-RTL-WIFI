/*
 * RTWCrashReporter.h - self-diagnosing crash logging for RTLWiFiBar.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/* Installs the uncaught-exception handler and the fatal-signal handlers
 * (SIGSEGV/SIGABRT/SIGBUS/SIGILL/SIGFPE/SIGTRAP + SIGTERM/SIGQUIT/SIGHUP/
 * SIGINT as "terminated by signal"). Every handler appends a "[CRASH]"
 * block to ~/Library/Logs/RTLWiFiBar.log and then re-raises with the
 * default disposition, so macOS still writes its own crash report into
 * ~/Library/Logs/DiagnosticReports and launchd sees a failure exit.
 * Call once at process start, before anything else. */
void RTWInstallCrashReporter(void);

/* Returns NO when another RTLWiFiBar process is already running (the
 * caller should exit silently). Fails OPEN: any internal error leaves the
 * app free to run. The lock lives on an flock()'d file, so a crashed
 * previous instance can never leave a stale lock behind. */
BOOL RTWAcquireSingleInstanceLock(void);

NS_ASSUME_NONNULL_END
