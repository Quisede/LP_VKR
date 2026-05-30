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

Создать полноценные `.app`-ярлыки `LMS Server` и `LMS Client` на рабочем столе macOS:

```text
scripts/install-macos-launchers.command
```

Или из терминала:

```bash
./scripts/install-macos-launchers.sh
```

Windows:

```text
scripts\start-server.bat
scripts\start-qt-client.bat
```

PowerShell-версия:

```powershell
.\scripts\start-server.ps1
.\scripts\start-qt-client.ps1
```

Создать ярлыки на рабочем столе Windows:

```powershell
.\scripts\install-windows-launchers.ps1
```

Linux:

```bash
./scripts/start-server.sh
./scripts/start-qt-client.sh
```

Создать `.desktop`-ярлыки на рабочем столе Linux:

```bash
./scripts/install-linux-launchers.sh
```

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

Пакеты приложения:

```bash
# macOS .app + zip
./scripts/package-macos-app.sh

# Linux .deb
./scripts/package-linux-deb.sh

# Linux AppImage, требуется linuxdeployqt
./scripts/package-linux-appimage.sh
```

Windows installer собирается на Windows:

```powershell
.\scripts\package-windows-installer.ps1
```

Скрипт использует `windeployqt`, а полноценный `.exe` installer создаёт через Inno Setup (`iscc`), если он установлен.

Если локальный Qt kit конфликтует с архитектурой macOS, обычный запуск `./scripts/start-qt-client.sh` попробует найти уже собранный Qt Creator `.app` в `client-qt/LMSClient/build/...` и запустить его.

Тестовые пользователи:

```text
student_demo / student123
student_anna / student123
student_oleg / student123
teacher1     / teacher123
teacher2     / teacher123
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
