#!/bin/bash
# sim_build.sh - Linux-sandbox full-flow simulation of the macOS-RTL-WIFI
# app+package build. Every real tool is replaced by a mock that ENFORCES the
# regressions we already hit on the user's machine:
#
#   1. mock clang REJECTS any compile without -isysroot pointing at an
#      existing directory (the "AppKit/AppKit.h file not found" failure).
#   2. mock clang REJECTS -framework CommonCrypto (not a macOS framework).
#   3. static checks reject the two macOS 26.2 SDK compile errors seen in
#      the field: +[NSImage imageWithSize:flipped:rect:drawingHandler:] and
#      bare C struct tags (ifaddrs / sockaddr_dl without `struct`).
#   4. wire-protocol constants in the app's RTWCompat.h are diffed against
#      the kext's Driver/RtWlanUCompat.h.
#   5. /Applications-install assertions on the package scripts and launch
#      agent (v8.7.16: the old utility is gone from the package entirely).
set -euo pipefail
cd "$(dirname "$0")/.."
ROOT="$PWD"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
MOCK="$WORK/mockbin"
mkdir -p "$MOCK" "$WORK/sdk/usr/lib" "$WORK/out"
PASS=0; FAIL=0
ok()   { PASS=$((PASS+1)); echo "  PASS: $1"; }
bad()  { FAIL=$((FAIL+1)); echo "  FAIL: $1"; }
assert_file()    { [ -e "$1" ] && ok "$2" || bad "$2 (missing: $1)"; }
assert_contains(){ grep -qF -- "$2" "$1" && ok "$3" || bad "$3"; }
assert_not_contains(){ grep -qF -- "$2" "$1" && bad "$3 (found: $2)" || ok "$3"; }

# ------------------------------------------------------------- mock toolchain
cat > "$MOCK/xcrun" <<'EOS'
#!/bin/bash
# mock xcrun: -f clang -> mock clang path; --sdk macosx --show-sdk-path -> SDK
if [ "$1" = "-f" ] || [ "$1" = "--find" ]; then
  echo "$MOCKBIN/clang"; exit 0
fi
for a in "$@"; do
  if [ "$prev" = "--sdk" ] && [ "$a" = "macosx" ] && [ "${@: -1}" = "--show-sdk-path" ]; then
    echo "$FAKE_SDK"; exit 0
  fi
  prev="$a"
done
if [ "${@: -1}" = "--show-sdk-path" ]; then echo "$FAKE_SDK"; exit 0; fi
echo "mock xcrun: unsupported args: $*" >&2; exit 1
EOS

cat > "$MOCK/clang" <<'EOS'
#!/bin/bash
# mock clang with regression guards.
isysroot=""
seen_sysroot=0
args=("$@")
for ((i=0; i<${#args[@]}; i++)); do
  if [ "${args[$i]}" = "-isysroot" ]; then
    seen_sysroot=1
    isysroot="${args[$((i+1))]}"
  fi
  if [ "${args[$i]}" = "-framework" ] && [ "${args[$((i+1))]}" = "CommonCrypto" ]; then
    echo "fatal error: mock clang: -framework CommonCrypto is not a macOS framework" >&2
    exit 1
  fi
done
if [ "$seen_sysroot" = 0 ] || [ ! -d "$isysroot" ]; then
  echo "fatal error: mock clang requires -isysroot <existing dir> (real clang has no default SDK search path)" >&2
  exit 1
fi
if [ -f "${args[${#args[@]}-1]}" ] && [[ "${args[${#args[@]}-1]}" == *.m ]]; then
  : # syntax-only probe
fi
# compile (-c file -o out) or link (-o out objs)
for ((i=0; i<${#args[@]}; i++)); do
  if [ "${args[$i]}" = "-o" ]; then
    out="${args[$((i+1))]}"
    mkdir -p "$(dirname "$out")"
    printf 'MOCKOBJ\n' > "$out"
    # real clang marks linked executables 0755 (matters for the zip exec bit)
    if [[ "$out" != *.o ]]; then chmod 0755 "$out"; fi
  fi
done
exit 0
EOS

cat > "$MOCK/xcodebuild" <<'EOS'
#!/bin/bash
# mock xcodebuild: produce the kext bundle shape Create_Build.tool expects.
SYMROOT="Build"; CONF="Release"
for ((i=0; i<$#; i++)); do
  [ "${!i}" = "SYMROOT" ] 2>/dev/null && :; done 2>/dev/null || true
while [ $# -gt 0 ]; do
  case "$1" in
    SYMROOT=*) SYMROOT="${1#SYMROOT=}";;
    -configuration) shift; CONF="$1";;
  esac; shift
done
D="$SYMROOT/$CONF/macOS-RTL-WIFI.kext/Contents/MacOS"
mkdir -p "$D"
printf 'MOCKKEXT\n' > "$D/macOS-RTL-WIFI"
cp /dev/null "$SYMROOT/$CONF/macOS-RTL-WIFI.kext/Contents/Info.plist" 2>/dev/null || \
  printf '<?xml version="1.0"?><plist version="1.0"><dict/></plist>' > "$SYMROOT/$CONF/macOS-RTL-WIFI.kext/Contents/Info.plist"
echo "** BUILD SUCCEEDED **"
exit 0
EOS

cat > "$MOCK/codesign" <<'EOS'
#!/bin/bash
exit 0
EOS

cat > "$MOCK/mkdir" <<'EOS'
#!/bin/bash
# mock mkdir: redirect macOS /Private/tmp into the sandbox work dir.
args=()
for a in "$@"; do
  args+=("${a//\/Private\/tmp/$SANDBOX_TMP}")
done
exec /bin/mkdir "${args[@]}"
EOS

cat > "$MOCK/pkgbuild" <<'EOS'
#!/bin/bash
out=""
for a in "$@"; do case "$a" in */*) case "$a" in *.pkg) out="$a";; esac;; esac; done
[ -n "$out" ] || { echo "mock pkgbuild: no .pkg output arg" >&2; exit 1; }
mkdir -p "$(dirname "$out")"
printf 'MOCKPKG' > "$out"
exit 0
EOS

cat > "$MOCK/pkgutil" <<'EOS'
#!/bin/bash
# --flatten src dst | --expand src dst
case "$1" in
  --flatten) mkdir -p "$(dirname "$3")"; printf 'MOCKFLATPKG' > "$3"; exit 0;;
  --expand)  mkdir -p "$3"; exit 0;;
esac
echo "mock pkgutil: unsupported $*" >&2; exit 1
EOS

chmod +x "$MOCK"/*
export FAKE_SDK="$WORK/sdk"
export MOCKBIN="$MOCK"
export SANDBOX_TMP="$WORK/tmp"
mkdir -p "$SANDBOX_TMP"
sed -i.bak "s|\$MOCKBIN|$MOCK|g; s|\$FAKE_SDK|$FAKE_SDK|g; s|\$SANDBOX_TMP|$SANDBOX_TMP|g" "$MOCK/xcrun" && rm -f "$MOCK/xcrun.bak"
sed -i.bak "s|\$SANDBOX_TMP|$SANDBOX_TMP|g" "$MOCK/mkdir" && rm -f "$MOCK/mkdir.bak"
PATH="$MOCK:$PATH"
export PATH

echo "== 1. static source checks =="
APP=App/RTLWiFiBar
if ! rg -q "imageWithSize" $APP/*.m 2>/dev/null; then ok "no removed NSImage drawingHandler factory"; else bad "removed NSImage drawingHandler factory present"; fi
if ! rg -q "ifaddrs" $APP/*.m $APP/*.h 2>/dev/null; then ok "no bare ifaddrs usage (getifaddrs dropped)"; else bad "ifaddrs usage present"; fi
if rg -q -- "-framework CommonCrypto|#import <CommonCrypto|#include <CommonCrypto>" $APP/*.m $APP/*.h Create_Build.tool 2>/dev/null; then bad "CommonCrypto referenced"; else ok "no CommonCrypto reference"; fi
if rg -q "RTWCOMPAT_SEL_OID_QUERY" $APP/DriverClient.m && rg -q "RTWCOMPAT_SEL_SURVEY_RECORD" $APP/DriverClient.m && rg -q "RTWCOMPAT_SEL_OID_SET" $APP/DriverClient.m; then ok "compat selectors used in DriverClient"; else bad "compat selectors missing"; fi

echo "== 2. wire constants must match the kext =="
for DEF in RTWCOMPAT_OID_FRAME_SIZE RTWCOMPAT_OID_DATA_MAX RTWCOMPAT_SURVEY_RECORD \
           RTWCOMPAT_REC_SSIDLEN_OFFSET RTWCOMPAT_REC_BSSID_OFFSET RTWCOMPAT_REC_CHANNEL_OFFSET \
           RTWCOMPAT_REC_SITE_ENCRY_OFFSET RTWCOMPAT_REC_AKMSUIT_OFFSET RTWCOMPAT_REC_SIGNALSTRENGTH_OFFSET \
           RTWCOMPAT_SID_SCAN_TRIGGER RTWCOMPAT_SID_WPA_KEY RTWCOMPAT_SID_ASSOC_TRIGGER \
           RTWCOMPAT_QID_NETWORK_STATUS RTWCOMPAT_QID_RSSI RTWCOMPAT_QID_SCAN_BUSY RTWCOMPAT_QID_SURVEY_COUNT; do
  V1=$(rg -o "#define $DEF\s+0x[0-9A-Fa-f]+u?" App/RTLWiFiBar/RTWCompat.h | rg -o "0x[0-9A-Fa-f]+" || true)
  TAIL=${DEF#RTWCOMPAT_}
  V2=""
  for KDEF in "RTWLANUC_$TAIL" "RTWLANU_$TAIL"; do   # the kext header mixes both prefixes
    V2=$(rg -o "#define $KDEF\s+0x[0-9A-Fa-f]+u?" Driver/RtWlanUCompat.h | rg -o "0x[0-9A-Fa-f]+" || true)
    [ -n "$V2" ] && break
  done
  if [ -n "$V1" ] && [ "$V1" = "$V2" ]; then ok "$DEF == kext ($V1)"; else bad "$DEF ($V1) != kext $TAIL ($V2)"; fi
done
if gcc -fsyntax-only -x c App/RTLWiFiBar/RTWCompat.h 2>"$WORK/syntax.log"; then ok "RTWCompat.h compiles as C (static asserts valid)"; else bad "RTWCompat.h C syntax: $(cat "$WORK/syntax.log")"; fi

echo "== 3. App build (folded into Create_Build.tool, section [3/3]) =="
# Build_App.tool is retired - the app is built inline by Create_Build.tool
# (see [3/3] there). The app binary, Info.plist and icns are still asserted
# below in section 4 via the RTLWiFiBar.zip payload inspection.

echo "== 4. Create_Build.tool under mocks =="
# seticon is a macOS binary; stub it on Linux and restore on exit.
SETICON="PKG/script/SETICON/seticon"
SETICON_BACKUP=""
if [ -f "$SETICON" ] && ! head -c2 "$SETICON" | grep -q '#!'; then
  SETICON_BACKUP="$WORK/seticon.real"
  cp -p "$SETICON" "$SETICON_BACKUP"
  printf '#!/bin/sh\nexit 0\n' > "$SETICON"
fi
restore_seticon() {
  if [ -n "$SETICON_BACKUP" ] && [ -f "$SETICON_BACKUP" ]; then
    cp -p "$SETICON_BACKUP" "$SETICON"
  fi
}
trap restore_seticon EXIT
if ./Create_Build.tool > "$WORK/create.log" 2>&1; then ok "Create_Build.tool exit 0"; else bad "Create_Build.tool failed: $(tail -8 "$WORK/create.log")"; fi
assert_file PKG/statusbarapp/RTLWiFiBar.zip "RTLWiFiBar.zip staged in payload"
if [ -s PKG/statusbarapp/RTLWiFiBar.zip ]; then
  if command -v python3 >/dev/null; then
    python3 - "$ROOT" <<'EOP'
import sys, zipfile
zf = zipfile.ZipFile("PKG/statusbarapp/RTLWiFiBar.zip")
names = zf.namelist()
assert any(n == "RTLWiFiBar.app/Contents/MacOS/RTLWiFiBar" for n in names), names[:10]
info = zf.getinfo("RTLWiFiBar.app/Contents/MacOS/RTLWiFiBar")
mode = info.external_attr >> 16
assert mode & 0o111, hex(mode)
print("  PASS: zip has RTLWiFiBar.app with exec bit", oct(mode & 0o777))
EOP
    [ $? = 0 ] && PASS=$((PASS+1)) || FAIL=$((FAIL+1))
  fi
fi
assert_file Build/Release/"Wireless USB macOS-RTL-WIFI.pkg" "final pkg placeholder produced"
assert_file Build/Release/RTLWiFiBar.app/Contents/MacOS/RTLWiFiBar "app copied into Build/Release"
[ -f Build/Release/"Support Donate.html" ] && ok "Support Donate.html staged into Build/Release" || bad "Support Donate.html missing from Build/Release"

echo "== 5. /Applications install + agent assertions =="
PI="PKG/script/script statusbarapp/postinstall"
assert_contains "$PI" 'ROOT_PATH="/Applications"' "postinstall installs into /Applications"
assert_contains "$PI" "RTLWiFiBar.zip" "postinstall extracts RTLWiFiBar.zip"
assert_not_contains "$PI" "StatusBarApp" "postinstall no longer touches the old utility"
assert_not_contains "$PI" "RtWlanHelper" "postinstall no longer restores RtWlanHelper"
assert_not_contains "$PI" "Application Support/WLAN" "postinstall no longer targets the WLAN support folder"
assert_contains "$PI" "KeepAlive" "postinstall agent plist keeps the app alive"
assert_contains "$PI" "ThrottleInterval" "postinstall agent plist carries a throttle interval"
assert_contains "$PI" "launchctl kickstart" "postinstall kickstarts the agent in the GUI session"
assert_contains "PKG/script/script statusbarapp/preinstall" "killall -c RTLWiFiBar" "preinstall stops RTLWiFiBar"
assert_not_contains "PKG/script/script statusbarapp/preinstall" "StatusBarApp" "preinstall no longer stops the old utility"
assert_not_contains "PKG/script/script statusbarapp/preinstall" "Application Support/WLAN" "preinstall no longer creates the WLAN support folder"
assert_contains "PKG/agents/Wlan.Software.plist" "/Applications/RTLWiFiBar.app/Contents/MacOS/RTLWiFiBar" "agent plist targets /Applications"
assert_not_contains "PKG/agents/Wlan.Software.plist" "Application Support/WLAN" "agent plist no longer targets the WLAN support folder"
assert_contains "PKG/script/script uninstall/postinstall" 'rm -rf "/Applications/RTLWiFiBar.app"' "uninstaller deletes /Applications/RTLWiFiBar.app"
assert_contains "PKG/script/script uninstall/postinstall" "killall -c RTLWiFiBar" "uninstaller stops RTLWiFiBar"
assert_not_contains "PKG/script/script uninstall/postinstall" "Application Support/WLAN" "uninstaller no longer references the WLAN support folder"
assert_contains "PKG/script/script uninstall/preinstall" "killall -c RTLWiFiBar" "uninstall preinstall stops RTLWiFiBar"
if [ -e PKG/statusbarapp/StatusBarApp.zip ]; then bad "old StatusBarApp.zip payload still in tree"; else ok "no StatusBarApp.zip payload"; fi
if [ -e PKG/statusbarapp/RtWlanHelper ]; then bad "old RtWlanHelper payload still in tree"; else ok "no RtWlanHelper payload"; fi
if command -v python3 >/dev/null; then
  python3 - <<'EOP' && PASS=$((PASS+1)) || FAIL=$((FAIL+1))
import re
agent = open("PKG/agents/Wlan.Software.plist").read()
m = re.search(r"<string>(/[^<]+)</string>", agent)
assert m and m.group(1) == "/Applications/RTLWiFiBar.app/Contents/MacOS/RTLWiFiBar", m and m.group(1)
assert "<key>KeepAlive</key>" in agent and "<key>ThrottleInterval</key>" in agent
post = open("PKG/script/script statusbarapp/postinstall").read()
# the heredoc plist inside postinstall must match the agents plist Program path
assert "RTLWiFiBar.app/Contents/MacOS/RTLWiFiBar" in post
assert "Application Support/WLAN" not in post and "StatusBarApp" not in post
print("  PASS: agent plist + embedded heredoc both target /Applications, KeepAlive on")
EOP
fi
assert_contains "Create_Build.tool" 'cp -Rp "./Build/App/RTLWiFiBar.app" ./Build/Release/' "Create_Build.tool copies the app into Build/Release"
assert_contains "Create_Build.tool" "mkdir -p ./PKG/statusbarapp" "Create_Build.tool stages PKG/statusbarapp"
assert_contains "Create_Build.tool" "RTLWiFiBar.zip" "Create_Build.tool zips the app into the payload"

echo "== 6. v8.7.17 startup auto-rejoin + profile persistence =="
APPD=App/RTLWiFiBar/AppDelegate.m
# The launch wipe is gone; the store is loaded and the saved network re-pinned.
assert_not_contains "$APPD" "[self.profiles resetAllProfiles]" "v8.7.17: the v8.7.13 launch wipe is gone"
assert_contains "$APPD" "[self.profiles savedSsids] ?: @[]" "launch loads the persisted profiles"
assert_contains "$APPD" "[self.profiles mostRecentlyUsedSsid]" "launch pins the most recently used saved network"
assert_contains "$APPD" 'startup: saved network \"%@\" loaded - auto-rejoin armed' "launch logs the armed auto-rejoin"
assert_contains "$APPD" "startup: no saved network - pick yours in Networks" "no-profile launch still tells the user what to do"
# The bounded auto-rejoin: one arm, survey-gated, manual join path.
assert_contains "$APPD" "- (void)maybeFireStartupAutoRejoinWithRecords:" "auto-rejoin consumes the survey records"
assert_contains "$APPD" 'startup auto-rejoin: \"%@\" is in the survey - joining with the saved password' "auto-rejoin fires only when the survey sees the network"
assert_contains "$APPD" "[self maybeFireStartupAutoRejoinWithRecords:records];" "performScan hands the completed survey to the auto-rejoin"
assert_contains "$APPD" "- (void)submitAutoRejoin:" "auto-rejoin has its own no-alert submit"
assert_contains "$APPD" "[self noteJoinSubmittedForEntry:entry passphrase:pass];" "auto-rejoin arms the same join monitor"
assert_contains "$APPD" "self.autoRejoinScansWaited >= 5" "auto-rejoin gives up after five empty scans"
assert_contains "$APPD" "startup auto-rejoin gave up on" "the give-up is logged, not silent"
assert_contains "$APPD" "- (void)standDownStartupAutoRejoin:" "auto-rejoin stands down for the user"
assert_contains "$APPD" "you picked a different network" "a user pick stands the auto-rejoin down"
assert_contains "$APPD" "you forgot this network" "Forget stands the auto-rejoin down"
# The persistence messages replaced the session-only wording.
assert_contains "$APPD" 'connection established - network \"%@\" saved (the app reconnects to it by itself after a restart; Forget removes it)' "save message documents the restart reconnect"
assert_not_contains "$APPD" "this session only; after a restart you type the password again" "retired session-only save message is gone"
assert_not_contains "$APPD" "stays saved for this session only" "retired session-only prompt text is gone"
assert_contains "App/RTLWiFiBar/ProfileStore.m" "kLastUsedKey = @\"lastUsed\"" "profile store stamps lastUsed"
assert_contains "App/RTLWiFiBar/ProfileStore.m" "- (nullable NSString *)mostRecentlyUsedSsid" "profile store exposes mostRecentlyUsedSsid"
assert_contains "App/RTLWiFiBar/ProfileStore.h" "mostRecentlyUsedSsid" "profile store header declares mostRecentlyUsedSsid"
assert_contains "App/RTLWiFiBar/Info.plist" "<string>8.7.17</string>" "app version is 8.7.17"

echo
echo "== summary: $PASS passed, $FAIL failed =="
[ "$FAIL" = 0 ]
