#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
CLIENT_DIR="$PROJECT_DIR/client-qt/LMSClient"
BUILD_DIR="${LMS_QT_BUILD_DIR:-$CLIENT_DIR/build/package-macos}"
DIST_DIR="$PROJECT_DIR/dist/macos"

mkdir -p "$BUILD_DIR" "$DIST_DIR"

cmake -S "$CLIENT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --config Release

APP_PATH="$(find "$BUILD_DIR" -maxdepth 3 -name 'LMSClient.app' -type d | head -n 1)"
if [[ -z "$APP_PATH" ]]; then
    echo "LMSClient.app not found in $BUILD_DIR" >&2
    exit 1
fi

if command -v macdeployqt >/dev/null 2>&1; then
    macdeployqt "$APP_PATH" -always-overwrite
else
    echo "macdeployqt not found. The app bundle was built, but Qt frameworks were not embedded." >&2
fi

rm -rf "$DIST_DIR/LMSClient.app"
cp -R "$APP_PATH" "$DIST_DIR/LMSClient.app"

if command -v ditto >/dev/null 2>&1; then
    ditto -c -k --sequesterRsrc --keepParent "$DIST_DIR/LMSClient.app" "$DIST_DIR/LMSClient-macos.zip"
fi

echo "macOS package is ready: $DIST_DIR/LMSClient.app"
