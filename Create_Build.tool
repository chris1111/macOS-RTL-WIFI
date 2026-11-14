#!/bin/bash
# Create_Build.tool — build macOS-RTL-WIFI: kext + installer package.
# This tree's own command, written fresh.
#
#   ./Create_Build.tool           release build + Wireless USB installer
#   ./Create_Build.tool debug     debug build + Wireless USB installer
#   ./Create_Build.tool -v        show full compiler output
#
# Needs only Xcode on the machine — no downloads, no external SDK.
# The packaging phase after the kext build uses pkgbuild/pkgutil,
# which ship with macOS.
set -euo pipefail
PARENTDIR=$(dirname "$0")
cd "$PARENTDIR"

P=macOS-RTL-WIFI.xcodeproj
T=macOS-RTL-WIFI
C=Release
VERBOSE=0
for a in "$@"; do
  case "$a" in
    debug)    C=Debug ;;
    -v|--verbose) VERBOSE=1 ;;
  esac
done
# SYMROOT is the build ROOT: xcodebuild appends $(CONFIGURATION) itself
# (TARGET_BUILD_DIR = $(SYMROOT)/$(CONFIGURATION)), so SYMROOT=Build puts the
# kext exactly at Build/Release/macOS-RTL-WIFI.kext.
S=Build
B=Build/$C

echo "[1/2] xcodebuild $C"
rm -rf "$S"
mkdir -p "$S"
if [ "$VERBOSE" = 1 ]; then
  xcodebuild -project "$P" -target "$T" -configuration "$C" build SYMROOT="$S" OBJROOT="$S/obj"
else
  R=$(xcodebuild -project "$P" -target "$T" -configuration "$C" build SYMROOT="$S" OBJROOT="$S/obj" 2>&1) || {
    printf '%s\n' "$R" | grep -E 'error:' || printf '%s\n' "$R" | tail -15
    exit 1
  }
fi

K="$B/$T.kext"
echo "[2/2] $K"
[ -d "$K" ] || K="$(find Build -type d -name "$T.kext" 2>/dev/null | head -n 1 || true)"
[ -n "$K" ] && [ -d "$K" ] || { echo "no kext produced"; exit 1; }
[ -s "$K/Contents/MacOS/$T" ] || { echo "no binary in kext Contents/MacOS"; exit 1; }
echo "ok"
echo "Create Package"
echo "= = = = = = = = = = = = = = = = = = = = = = = = =  "
echo "Wireless USB macOS-RTL-WIFI "
echo "= = = = = = = = = = = = = = = = = = = = = = = = =  "
sleep 1
# Remove old build
rm -rf ./InstallerWIFI
rm -rf /Private/tmp/Package-DIR
rm -rf ./*.pkg
rm -rf ./PKG/efifolderclover
rm -rf ./PKG/efifolderoc
sleep 1
# Create necessary folder
mkdir -p ./InstallerWIFI
mkdir -p ./InstallerWIFI/BUILD-PACKAGE
mkdir -p /Private/tmp/Package-DIR
mkdir -p ./PKG/uninstall
mkdir -p ./PKG/efifolderclover/EFI/CLOVER/kexts/Other
mkdir -p ./PKG/efifolderoc/EFI/OC/Kexts
sleep 1
# Copy kext to bootloader
cp -Rp ./Build/Release/macOS-RTL-WIFI.kext ./PKG/efifolderclover/EFI/CLOVER/kexts/Other
cp -Rp ./Build/Release/macOS-RTL-WIFI.kext ./PKG/efifolderoc/EFI/OC/Kexts
sleep 1
# Create the Packages with pkgbuild
pkgbuild --root ./PKG/statusbarapp --scripts ./PKG/Script/"script statusbarapp" --identifier com.109Driver.WirelessUSBmacOS-RTL-WIFI.statusbarapp.pkg --version 1 --install-location /Private/tmp/ ./InstallerWIFI/BUILD-PACKAGE/statusbarapp.pkg
pkgbuild --root ./PKG/agents --identifier com.109Driver.WirelessUSBmacOS-RTL-WIFI.Agents.pkg --version 1 --install-location /Library/LaunchAgents ./InstallerWIFI/BUILD-PACKAGE/agents.pkg
pkgbuild --root ./PKG/efifolderoc --scripts ./PKG/Script/"script oc" --identifier com.109Driver.WirelessUSBmacOS-RTL-WIFI.EFIFOLDEROC.pkg --version 1 --install-location /Private/tmp ./InstallerWIFI/BUILD-PACKAGE/efifolderoc.pkg
pkgbuild --root ./PKG/efifolderclover --scripts ./PKG/Script/"script clover" --identifier com.109Driver.WirelessUSBmacOS-RTL-WIFI.EFIFOLDERCLOVER.pkg --version 1 --install-location /Private/tmp ./InstallerWIFI/BUILD-PACKAGE/efifolderclover.pkg
pkgbuild --root ./PKG/uninstall --scripts ./PKG/Script/"script uninstall" --identifier com.109Driver.WirelessUSBmacOS-RTL-WIFI.uninstall.pkg --version 1 --install-location / ./InstallerWIFI/BUILD-PACKAGE/uninstall.pkg
sleep 2
# Expend the Packages with pkgutil
pkgutil --expand ./InstallerWIFI/BUILD-PACKAGE/statusbarapp.pkg /tmp/Package-DIR/statusbarapp.pkg
pkgutil --expand ./InstallerWIFI/BUILD-PACKAGE/agents.pkg /tmp/Package-DIR/agents.pkg
pkgutil --expand ./InstallerWIFI/BUILD-PACKAGE/efifolderoc.pkg /tmp/Package-DIR/efifolderoc.pkg
pkgutil --expand ./InstallerWIFI/BUILD-PACKAGE/efifolderclover.pkg /tmp/Package-DIR/efifolderclover.pkg
pkgutil --expand ./InstallerWIFI/BUILD-PACKAGE/uninstall.pkg /tmp/Package-DIR/uninstall.pkg
echo "
= = = = = = = = = = = = = = = = = = = = = = = = =
Create the final Packages with Productbuild "
sleep 2
# Copy resources and distribution
cp -r ./PKG/Resources/Distribution /tmp/Package-DIR
cp -r ./PKG/Resources/Resources /tmp/Package-DIR
sleep 1
# Flatten the Packages with pkgutil
pkgutil --flatten /Private/tmp/Package-DIR ./Build/Release/"Wireless USB macOS-RTL-WIFI.pkg"
sleep 1
# Copy the PayPal donation page next to the package
mkdir -p ./Build/Release
if [ -f "./Support Donate.html" ]; then
  cp -f "./Support Donate.html" ./Build/Release/
  echo "Support Donate.html copied to ./Build/Release"
fi
# Set the PayPal custom icon on the donation page (osxutils seticon)
if [ -f "./Build/Release/Support Donate.html" ] && [ -x "./PKG/script/SETICON/seticon" ] && [ -f "./PKG/script/SETICON/Paypal.icns" ]; then
  ./PKG/script/SETICON/seticon -d ./PKG/script/SETICON/Paypal.icns ./Build/Release/"Support Donate.html"
  echo "PayPal custom icon set on ./Build/Release/Support Donate.html"
fi
echo "= = = = = = = = = = = = = = = = = = = = = = = = =  "
echo "Wireless USB macOS-RTL-WIFI.pkg
Completed Done!   "
echo "= = = = = = = = = = = = = = = = = = = = = = = = =  "
# Remove old build
rm -rf ./InstallerWIFI
rm -rf ./PKG/efifolderclover
rm -rf ./PKG/efifolderoc
# Open release (guarded: headless CI runners have no GUI session)
open ./Build/Release 2>/dev/null || true

