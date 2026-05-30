#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
CLIENT_DIR="$PROJECT_DIR/client-qt/LMSClient"
BUILD_DIR="${LMS_QT_BUILD_DIR:-$CLIENT_DIR/build/package-linux-deb}"
DIST_DIR="$PROJECT_DIR/dist/linux"

mkdir -p "$BUILD_DIR" "$DIST_DIR"

cmake -S "$CLIENT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --config Release
cmake --build "$BUILD_DIR" --target package

find "$BUILD_DIR" -maxdepth 1 -name '*.deb' -exec cp {} "$DIST_DIR/" \;
echo "DEB package output: $DIST_DIR"
