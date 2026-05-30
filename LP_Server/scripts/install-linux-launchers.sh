#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
DESKTOP_DIR="${XDG_DESKTOP_DIR:-$HOME/Desktop}"
ICON_PATH="$PROJECT_DIR/client-qt/LMSClient/resources/icons/lms-app.svg"

mkdir -p "$DESKTOP_DIR"

cat > "$DESKTOP_DIR/LMS Server.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=LMS Server
Comment=Build and start LMS backend
Icon=$ICON_PATH
Terminal=true
Exec=bash -lc 'cd "$PROJECT_DIR" && ./scripts/start-server.sh'
Categories=Development;Education;
EOF

cat > "$DESKTOP_DIR/LMS Client.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=LMS Client
Comment=Build and start LMS Qt client
Icon=$ICON_PATH
Terminal=true
Exec=bash -lc 'cd "$PROJECT_DIR" && ./scripts/start-qt-client.sh'
Categories=Development;Education;
EOF

chmod +x "$DESKTOP_DIR/LMS Server.desktop" "$DESKTOP_DIR/LMS Client.desktop"
echo "Linux launchers created on: $DESKTOP_DIR"
