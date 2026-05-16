#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
CLIENT_DIR="$PROJECT_DIR/client-qt/LMSClient"
BUILD_DIR="${LMS_QT_BUILD_DIR:-$CLIENT_DIR/build/one-click-macos}"
API_URL="${LMS_API_BASE_URL:-http://localhost:8080}"
BUILD_ONLY=0
if [[ "${1:-}" == "--build-only" ]]; then
    BUILD_ONLY=1
fi

pause_if_needed() {
    if [[ "${LMS_KEEP_OPEN:-0}" == "1" ]]; then
        echo
        read -r -p "Нажмите Enter, чтобы закрыть окно..."
    fi
}

find_existing_qt_app() {
    find "$CLIENT_DIR/build" -path "*/LMSClient.app/Contents/MacOS/LMSClient" -type f -perm +111 2>/dev/null \
        | sort \
        | tail -n 1
}

if ! command -v cmake >/dev/null 2>&1; then
    echo "CMake не найден. Установите CMake или откройте проект через Qt Creator."
    pause_if_needed
    exit 1
fi

echo "==> Проверяем backend: $API_URL"
if ! curl -fsS "$API_URL/api/health" >/dev/null 2>&1; then
    echo "Backend не отвечает. Сначала запустите scripts/start-server.command или scripts/start-server.sh."
    echo "Клиент всё равно будет запущен, но данные появятся только после старта backend."
fi

CMAKE_PREFIX_ARGS=()
if [[ -n "${QT_CMAKE_PREFIX_PATH:-}" && -d "${QT_CMAKE_PREFIX_PATH:-}" ]]; then
    CMAKE_PREFIX_ARGS+=("-DCMAKE_PREFIX_PATH=$QT_CMAKE_PREFIX_PATH")
elif [[ -n "${CMAKE_PREFIX_PATH:-}" ]]; then
    CMAKE_PREFIX_ARGS+=("-DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH")
else
    for candidate in \
        "$HOME/QT_C++/6.7.2/macos" \
        "$HOME/Qt/6.7.2/macos" \
        "$HOME/Qt/6.7.3/macos" \
        "$HOME/Qt/6.8.0/macos" \
        "$HOME/Qt/6.8.1/macos" \
        "$HOME/Qt/6.9.0/macos"; do
        if [[ -f "$candidate/lib/cmake/Qt6/Qt6Config.cmake" ]]; then
            CMAKE_PREFIX_ARGS+=("-DCMAKE_PREFIX_PATH=$candidate")
            break
        fi
    done
fi

echo
echo "==> Собираем Qt-клиент..."
BUILD_OK=1
if ! cmake -S "$CLIENT_DIR" -B "$BUILD_DIR" "${CMAKE_PREFIX_ARGS[@]}"; then
    BUILD_OK=0
elif ! cmake --build "$BUILD_DIR" --config Debug; then
    BUILD_OK=0
fi

if [[ "$BUILD_ONLY" == "1" ]]; then
    if [[ "$BUILD_OK" == "1" ]]; then
        echo "Qt-клиент успешно собран."
        pause_if_needed
        exit 0
    fi

    echo "Qt-клиент не удалось собрать через локальный CMake/Qt kit."
    echo "Если проект уже собирается в Qt Creator, можно запускать кнопку без --build-only: она попробует найти готовый .app."
    pause_if_needed
    exit 1
fi

APP_BIN="$BUILD_DIR/LMSClient.app/Contents/MacOS/LMSClient"
if [[ ! -x "$APP_BIN" ]]; then
    APP_BIN="$BUILD_DIR/LMSClient"
fi

if [[ "$BUILD_OK" != "1" || ! -x "$APP_BIN" ]]; then
    FALLBACK_APP="$(find_existing_qt_app || true)"
    if [[ -n "$FALLBACK_APP" ]]; then
        echo
        echo "Локальная сборка через CMake не прошла, запускаем уже собранный Qt Creator app:"
        echo "$FALLBACK_APP"
        APP_BIN="$FALLBACK_APP"
    fi
fi

if [[ ! -x "$APP_BIN" ]]; then
    echo "Не удалось найти собранный LMSClient."
    echo "Папка сборки: $BUILD_DIR"
    echo "Откройте client-qt/LMSClient/CMakeLists.txt в Qt Creator, соберите проект один раз и повторите запуск кнопки."
    pause_if_needed
    exit 1
fi

if [[ "${LMS_DRY_RUN:-0}" == "1" ]]; then
    echo "Qt-клиент готов к запуску:"
    echo "$APP_BIN"
    pause_if_needed
    exit 0
fi

echo
echo "==> Запускаем Qt-клиент..."
echo "API: $API_URL"
export LMS_API_BASE_URL="$API_URL"
"$APP_BIN"

pause_if_needed
