#!/usr/bin/env bash
set -euo pipefail

ROLE="${1:-teacher}"
PAGE="${2:-dashboard}"
COURSE_TITLE="${3:-}"
OUT_NAME="${4:-${ROLE}_${PAGE}.png}"

case "${ROLE}" in
  student)
    LOGIN="student_demo"
    PASSWORD="student123"
    ;;
  teacher)
    LOGIN="teacher1"
    PASSWORD="teacher123"
    ;;
  admin)
    LOGIN="admin"
    PASSWORD="admin123"
    ;;
  login)
    LOGIN=""
    PASSWORD=""
    ;;
  *)
    echo "Unknown role: ${ROLE}" >&2
    exit 2
    ;;
esac

ARTIFACTS_DIR="${ARTIFACTS_DIR:-/Users/daniil.druzinin/LP_VKR/LP_Server/LP_Server/artifacts/qt-client}"
mkdir -p "${ARTIFACTS_DIR}"

if [[ "${ROLE}" == "login" ]]; then
  exec docker compose --profile qt -f /Users/daniil.druzinin/LP_VKR/LP_Server/LP_Server/docker-compose.yml run --rm \
    -e LMS_AUTOMATION=1 \
    -e LMS_AUTOMATION_PAGE=login \
    -e LMS_AUTOMATION_SCREENSHOT_PATH="/artifacts/${OUT_NAME}" \
    qt_client run-automation
fi

exec docker compose --profile qt -f /Users/daniil.druzinin/LP_VKR/LP_Server/LP_Server/docker-compose.yml run --rm \
  -e LMS_AUTOMATION=1 \
  -e LMS_AUTOMATION_LOGIN="${LOGIN}" \
  -e LMS_AUTOMATION_PASSWORD="${PASSWORD}" \
  -e LMS_AUTOMATION_PAGE="${PAGE}" \
  -e LMS_AUTOMATION_COURSE_TITLE="${COURSE_TITLE}" \
  -e LMS_AUTOMATION_SCREENSHOT_PATH="/artifacts/${OUT_NAME}" \
  qt_client run-automation
