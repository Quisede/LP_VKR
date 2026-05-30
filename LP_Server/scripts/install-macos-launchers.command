#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
"$SCRIPT_DIR/install-macos-launchers.sh"
echo
read -r -p "Нажмите Enter для выхода..."
