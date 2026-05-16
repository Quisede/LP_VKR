#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LMS_KEEP_OPEN=1
export LMS_FOLLOW_LOGS=1
"$SCRIPT_DIR/start-server.sh"
