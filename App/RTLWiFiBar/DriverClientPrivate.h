/*
 * DriverClientPrivate.h - writable plumbing shared by RTLWiFiBar units.
 *
 * DriverClient.h deliberately exposes RTWDriverStatus as a read-only
 * snapshot. Two translation units need to build one, though: DriverClient.m
 * (the 1 Hz poll path) and AppDelegate.m (the offline "driver unreachable"
 * snapshot). This header redeclares the properties readwrite; only
 * DriverClient.m owns the @implementation, so the accessors are synthesized
 * exactly once and every other unit just links against them.
 *
 * App-private - never ship in a consumer-facing interface.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#import "DriverClient.h"

NS_ASSUME_NONNULL_BEGIN

@interface RTWDriverStatus ()
@property (nonatomic, assign, readwrite) BOOL driverReachable;
@property (nonatomic, assign, readwrite) BOOL linked;
@property (nonatomic, assign, readwrite) BOOL securing;
@property (nonatomic, copy, readwrite, nullable) NSString *ssid;
@property (nonatomic, assign, readwrite) uint32_t channel;
@property (nonatomic, assign, readwrite) int32_t rssiPercent;
@property (nonatomic, copy, readwrite, nullable) NSString *driverVersion;
@property (nonatomic, copy, readwrite) NSString *lastError;
@end

NS_ASSUME_NONNULL_END
