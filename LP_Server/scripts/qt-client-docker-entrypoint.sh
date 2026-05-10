#!/usr/bin/env bash
set -euo pipefail

MODE="${1:-build}"
CLIENT_DIR="/workspace/client-qt/LMSClient"

if [[ -n "${BUILD_DIR:-}" ]]; then
  EFFECTIVE_BUILD_DIR="${BUILD_DIR}"
elif [[ "${MODE}" == "run-automation" ]]; then
  EFFECTIVE_BUILD_DIR="/tmp/lmsclient-build-${RANDOM}-${RANDOM}"
else
  EFFECTIVE_BUILD_DIR="/workspace/client-qt/LMSClient/build-docker"
fi

mkdir -p "${EFFECTIVE_BUILD_DIR}"

cmake -S "${CLIENT_DIR}" -B "${EFFECTIVE_BUILD_DIR}" -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build "${EFFECTIVE_BUILD_DIR}" --parallel

if [[ "${MODE}" == "build" ]]; then
  exit 0
fi

if [[ "${MODE}" == "run-automation" ]]; then
  mkdir -p /artifacts
  export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"
  exec "${EFFECTIVE_BUILD_DIR}/LMSClient"
fi

echo "Unknown mode: ${MODE}" >&2
exit 1
