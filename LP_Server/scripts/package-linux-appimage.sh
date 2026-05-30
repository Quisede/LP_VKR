#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
CLIENT_DIR="$PROJECT_DIR/client-qt/LMSClient"
BUILD_DIR="${LMS_QT_BUILD_DIR:-$CLIENT_DIR/build/package-linux-appimage}"
APPDIR="$PROJECT_DIR/dist/appimage/AppDir"
DIST_DIR="$PROJECT_DIR/dist/appimage"

if ! command -v linuxdeployqt >/dev/null 2>&1; then
    echo "linuxdeployqt not found. Install linuxdeployqt to build AppImage." >&2
    exit 1
fi

mkdir -p "$BUILD_DIR" "$APPDIR/usr/bin" "$APPDIR/usr/share/applications" "$APPDIR/usr/share/icons/hicolor/scalable/apps"

cmake -S "$CLIENT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --config Release

cp "$BUILD_DIR/LMSClient" "$APPDIR/usr/bin/LMSClient"
cp "$CLIENT_DIR/resources/icons/lms-app.svg" "$APPDIR/usr/share/icons/hicolor/scalable/apps/lms-client.svg"

cat > "$APPDIR/usr/share/applications/lms-client.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=LMS Client
Exec=LMSClient
Icon=lms-client
Categories=Education;Development;
EOF

linuxdeployqt "$APPDIR/usr/share/applications/lms-client.desktop" -appimage
mkdir -p "$DIST_DIR"
find . -maxdepth 1 -name '*.AppImage' -exec mv {} "$DIST_DIR/" \;
echo "AppImage output: $DIST_DIR"
