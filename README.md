# LMS Platform

Учебная LMS-платформа для дипломного проекта: backend на C++ с PostgreSQL и Qt-клиентом для студента, преподавателя и администратора.

## Быстрый запуск

Из папки проекта:

```bash
cd LP_Server
./scripts/start-server.sh
./scripts/start-qt-client.sh
```

На macOS можно запускать “одной кнопкой” двойным кликом:

```text
LP_Server/scripts/start-server.command
LP_Server/scripts/start-qt-client.command
```

Сначала запускается сервер, затем Qt-клиент.

## Docker backend

```bash
cd LP_Server
docker compose up --build -d db app
```

Проверка API:

```bash
curl http://localhost:8080/api/health
```

Если база уже была создана, `init.sql` автоматически не переигрывается. Для полного сброса демо-данных:

```bash
cd LP_Server
docker compose down -v
docker compose up --build -d db app
```

Если нужно применить обновлённые демо-данные без удаления volume:

```bash
cd LP_Server
docker compose exec -T db psql -U postgres -d lms -f /docker-entrypoint-initdb.d/init.sql
```

## Qt-клиент

Основной CMake-файл клиента:

```text
LP_Server/client-qt/LMSClient/CMakeLists.txt
```

Сборка без запуска окна:

```bash
cd LP_Server
./scripts/start-qt-client.sh --build-only
```

Если локальный Qt kit на macOS конфликтует с архитектурой, скрипт запуска попробует найти уже собранный `.app` из Qt Creator.

## Демо-пользователи

```text
student_demo / student123
teacher1     / teacher123
admin        / admin123
```

Профили демо-пользователей заполнены ФИО, группой или подразделением, почтой и телефоном. Преподаватель в карточках курсов отображается по ФИО.

## Роли

Student:

- просмотр курсов, уроков, материалов и видео;
- прохождение тестов;
- история результатов;
- личный кабинет и смена пароля.

Teacher:

- управление своими курсами;
- создание и редактирование уроков, материалов и тестов;
- загрузка файлов материалов;
- просмотр студентов и аналитики курса;
- личный кабинет и смена пароля.

Admin:

- обзор системы;
- управление пользователями;
- аудит действий;
- просмотр структуры курсов;
- личный кабинет и смена пароля.

## Полезные команды

Логи backend:

```bash
cd LP_Server
docker compose logs -f app
```

Принудительная пересборка backend:

```bash
cd LP_Server
LMS_FORCE_REBUILD=1 ./scripts/start-server.sh
```

Автоматический скрин Qt для проверки интерфейса:

```bash
cd LP_Server
./scripts/qt-client-capture.sh student courses "" student_courses.png
```
