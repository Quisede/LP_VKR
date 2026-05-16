#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

FOLLOW_LOGS=0
if [[ "${1:-}" == "--follow" || "${LMS_FOLLOW_LOGS:-0}" == "1" ]]; then
    FOLLOW_LOGS=1
fi

if ! command -v docker >/dev/null 2>&1; then
    echo "Docker не найден. Установите Docker Desktop и запустите его."
    exit 1
fi

cd "$PROJECT_DIR"

if [[ "${LMS_FORCE_REBUILD:-0}" != "1" ]] && curl -fsS "http://localhost:8080/api/health" >/tmp/lms-health.json 2>/dev/null; then
    echo "Backend уже запущен: http://localhost:8080"
    cat /tmp/lms-health.json
    echo
else
    echo "==> Запускаем PostgreSQL и backend..."
    docker compose up --build -d db app
fi

echo
echo "==> Проверяем состояние контейнеров..."
docker compose ps

echo
echo "==> Проверяем backend health-check..."
for attempt in {1..30}; do
    if curl -fsS "http://localhost:8080/api/health" >/tmp/lms-health.json 2>/dev/null; then
        echo "Backend доступен: http://localhost:8080"
        cat /tmp/lms-health.json
        echo
        break
    fi

    if [[ "$attempt" == "30" ]]; then
        echo "Backend пока не ответил. Проверьте логи: docker compose logs app"
        exit 1
    fi

    sleep 1
done

echo
echo "Тестовые аккаунты:"
echo "  student_demo / student123"
echo "  teacher1     / teacher123"
echo "  admin        / admin123"

if [[ "$FOLLOW_LOGS" == "1" ]]; then
    echo
    echo "==> Показываем логи backend. Чтобы закрыть окно логов: Ctrl+C."
    echo "Контейнеры при этом продолжат работать."
    docker compose logs -f app
fi

if [[ "${LMS_KEEP_OPEN:-0}" == "1" ]]; then
    echo
    read -r -p "Нажмите Enter, чтобы закрыть окно..."
fi
