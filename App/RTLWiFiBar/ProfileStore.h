/*
 * ProfileStore.h - remembers join profiles per SSID in the app's OWN
 * profile file. No macOS Keychain is involved, ever.
 *
 * Rebuilding or re-signing the app binary loses the login Keychain ACL for
 * an old item, and macOS then prompts "RTLWiFiBar wants to access your
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
 *       remembered (1 = saved after the user connected with its password)
 *
 * The file lives in the user's own Library with 0700/0600 permissions, the
 * passphrase is never written in clear, and nothing here ever touches the
 * Security framework - so no keychain dialog can ever appear.
 *
 * v8.7.7 save rule: a profile is written only after the user selected the
 * network, entered its password in the prompt, and the link came up.
 *
 * v8.7.17 persistence rule: profiles SURVIVE a restart now (the app no
 * longer wipes them at launch - the user asked the saved network back).
 * Every save stamps "lastUsed" so the app knows WHICH network to
 * auto-rejoin once after the next reboot (mostRecentlyUsedSsid); the
 * link-up commit re-stamps it, so the network the app reconnects to is
 * exactly the one last used. Forget deletes a profile for real - a
 * forgotten network can never be rejoined.
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
/* Wipe every profile and delete the store file (kept for a manual
 * reset - the app no longer calls this at launch, v8.7.17). */
- (void)resetAllProfiles;
/* Every SSID with a remembered profile (alphabetical). */
- (nullable NSArray<NSString *> *)savedSsids;
/* The SSID with the newest lastUsed stamp (nil when the store is
 * empty) - the one network the v8.7.17 startup auto-rejoin arms for. */
- (nullable NSString *)mostRecentlyUsedSsid;
@end

NS_ASSUME_NONNULL_END
