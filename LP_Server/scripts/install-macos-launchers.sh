#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
DESKTOP_DIR="$HOME/Desktop"
ICON_SVG="$PROJECT_DIR/client-qt/LMSClient/resources/icons/lms-app.svg"
ICON_ICNS="$PROJECT_DIR/client-qt/LMSClient/resources/icons/lms-app.icns"

escape_for_applescript() {
    local value="$1"
    value="${value//\\/\\\\}"
    value="${value//\"/\\\"}"
    printf '%s' "$value"
}

make_icns_if_possible() {
    if [[ -f "$ICON_ICNS" ]]; then
        return
    fi

    if ! command -v qlmanage >/dev/null 2>&1 || ! command -v sips >/dev/null 2>&1 || ! command -v iconutil >/dev/null 2>&1; then
        return
    fi

    local work_dir
    work_dir="$(mktemp -d)"
    local iconset="$work_dir/lms-app.iconset"
    mkdir -p "$iconset"

    if ! qlmanage -t -s 1024 -o "$work_dir" "$ICON_SVG" >/dev/null 2>&1; then
        rm -rf "$work_dir"
        return
    fi

    local preview_png
    preview_png="$(find "$work_dir" -maxdepth 1 -name '*.png' | head -n 1)"
    if [[ -z "$preview_png" ]]; then
        rm -rf "$work_dir"
        return
    fi

    sips -z 16 16 "$preview_png" --out "$iconset/icon_16x16.png" >/dev/null
    sips -z 32 32 "$preview_png" --out "$iconset/icon_16x16@2x.png" >/dev/null
    sips -z 32 32 "$preview_png" --out "$iconset/icon_32x32.png" >/dev/null
    sips -z 64 64 "$preview_png" --out "$iconset/icon_32x32@2x.png" >/dev/null
    sips -z 128 128 "$preview_png" --out "$iconset/icon_128x128.png" >/dev/null
    sips -z 256 256 "$preview_png" --out "$iconset/icon_128x128@2x.png" >/dev/null
    sips -z 256 256 "$preview_png" --out "$iconset/icon_256x256.png" >/dev/null
    sips -z 512 512 "$preview_png" --out "$iconset/icon_256x256@2x.png" >/dev/null
    sips -z 512 512 "$preview_png" --out "$iconset/icon_512x512.png" >/dev/null
    sips -z 1024 1024 "$preview_png" --out "$iconset/icon_512x512@2x.png" >/dev/null

    iconutil -c icns "$iconset" -o "$ICON_ICNS" >/dev/null 2>&1 || true
    rm -rf "$work_dir"
}

create_launcher_app() {
    local app_name="$1"
    local command="$2"
    local bundle_id="$3"
    local app_dir="$DESKTOP_DIR/$app_name.app"
    local contents_dir="$app_dir/Contents"
    local macos_dir="$contents_dir/MacOS"
    local resources_dir="$contents_dir/Resources"

    mkdir -p "$macos_dir" "$resources_dir"

    if [[ -f "$ICON_ICNS" ]]; then
        cp "$ICON_ICNS" "$resources_dir/lms-app.icns"
    fi

    cat > "$contents_dir/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>launcher</string>
    <key>CFBundleIdentifier</key>
    <string>$bundle_id</string>
    <key>CFBundleName</key>
    <string>$app_name</string>
    <key>CFBundleDisplayName</key>
    <string>$app_name</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.14</string>
EOF

    if [[ -f "$ICON_ICNS" ]]; then
        cat >> "$contents_dir/Info.plist" <<EOF
    <key>CFBundleIconFile</key>
    <string>lms-app</string>
EOF
    fi

    cat >> "$contents_dir/Info.plist" <<EOF
</dict>
</plist>
EOF

    local escaped_command
    escaped_command="$(escape_for_applescript "$command")"

    cat > "$macos_dir/launcher" <<EOF
#!/usr/bin/env bash
osascript \\
  -e 'tell application "Terminal" to activate' \\
  -e 'tell application "Terminal" to do script "$escaped_command"'
EOF

    chmod +x "$macos_dir/launcher"
}

make_icns_if_possible

SERVER_COMMAND="cd \"$PROJECT_DIR\" && export LMS_KEEP_OPEN=1 && export LMS_FOLLOW_LOGS=1 && ./scripts/start-server.sh"
CLIENT_COMMAND="cd \"$PROJECT_DIR\" && export LMS_KEEP_OPEN=1 && ./scripts/start-qt-client.sh"

create_launcher_app "LMS Server" "$SERVER_COMMAND" "local.lms.server.launcher"
create_launcher_app "LMS Client" "$CLIENT_COMMAND" "local.lms.client.launcher"

echo "macOS launchers created on: $DESKTOP_DIR"
echo "Use LMS Server.app first, then LMS Client.app."
