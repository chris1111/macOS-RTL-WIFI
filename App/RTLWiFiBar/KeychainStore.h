/*
 * KeychainStore.h - remembers join profiles per SSID WITHOUT the Keychain.
 *
 * Rebuilding or re-signing the app binary loses the login Keychain ACL for
 * the old item, and macOS then prompts "RTLWiFiBar wants to access your
 * keychain" on every first join. The stock Realtek utility avoids that by
 * keeping its own ProfilesList.plist, so this store does the same:
 *
 *   ~/Library/Application Support/RTLWiFiBar/Profiles.plist
 *
 *   networks
 *     "<SSID>"
 *       pass       (XOR 0x5A + base64 - obfuscation, not encryption)
 *       auth       (utility auth enum, e.g. RTWCOMPAT_AUTH_WPA2PSK)
 *       channel    (survey channel at remember time, 0 = unknown)
 *       remembered (1 = the user ticked "Remember this network")
 *
 * The file lives in the user's own Library with 0700/0600 permissions, the
 * passphrase is never written in clear, and nothing here ever touches the
 * Security framework - so no keychain dialog can ever appear.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface RTWProfileStore : NSObject
/* The stored passphrase for an SSID, or nil when no profile exists. */
- (nullable NSString *)passphraseForSsid:(NSString *)ssid;
/* YES when a remembered profile exists for this SSID. */
- (BOOL)hasProfileForSsid:(NSString *)ssid;
/* The auth mode saved with the profile (defaults to WPA2-PSK). */
- (uint8_t)authModeForSsid:(NSString *)ssid;
/* The channel saved with the profile (0 = unknown). */
- (uint32_t)channelForSsid:(NSString *)ssid;
/* Create/update a remembered profile. Passphrase may be empty for open
 * networks (the profile then only carries auth/channel). */
- (BOOL)rememberNetwork:(NSString *)ssid
             passphrase:(nullable NSString *)passphrase
               authMode:(uint8_t)authMode
                channel:(uint32_t)channel;
/* Legacy shim kept for older call sites: saves a WPA2-PSK profile. */
- (BOOL)savePassphrase:(NSString *)passphrase forSsid:(NSString *)ssid;
/* Remove a profile. Returns YES when nothing was stored, too. */
- (BOOL)forgetSsid:(NSString *)ssid;
/* Wipe every profile and delete the store file (one-time v8.7 reset). */
- (void)resetAllProfiles;
/* Every SSID with a remembered profile (alphabetical). */
- (nullable NSArray<NSString *> *)savedSsids;
@end

NS_ASSUME_NONNULL_END
