# LMS Platform Backend + Qt Client

Основная инструкция по проекту лежит в корневом README:

```text
../README.md
```

Короткий запуск из этой папки:

```bash
docker compose up --build -d db app
```

Запуск “одной кнопкой” на macOS:

```text
scripts/start-server.command
scripts/start-qt-client.command
```

Порядок простой: сначала двойной клик по `start-server.command`, затем двойной клик по `start-qt-client.command`.

Те же команды из терминала:

```bash
./scripts/start-server.sh
./scripts/start-qt-client.sh
```

Если нужно смотреть live-логи backend:

```bash
./scripts/start-server.sh --follow
```

Принудительно пересобрать backend-контейнер:

```bash
LMS_FORCE_REBUILD=1 ./scripts/start-server.sh
```

Проверить сборку Qt без запуска окна:

```bash
./scripts/start-qt-client.sh --build-only
```

Если локальный Qt kit конфликтует с архитектурой macOS, обычный запуск `./scripts/start-qt-client.sh` попробует найти уже собранный Qt Creator `.app` в `client-qt/LMSClient/build/...` и запустить его.

Тестовые пользователи:

```text
student_demo / student123
teacher1     / teacher123
admin        / admin123
```

Демо-данные включают заполненные профили, курсы с ФИО преподавателя, подробные уроки, материалы, ссылки, видео и тесты.

Qt-клиент открывается через:

```text
client-qt/LMSClient/CMakeLists.txt
```

Автоматический скрин Qt:

```bash
./scripts/qt-client-capture.sh admin users "" admin_users.png
```

Сброс базы и повторное применение `init.sql`:

```bash
docker compose down -v
docker compose up --build -d db app
```
