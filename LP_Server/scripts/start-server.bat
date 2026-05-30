@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%.."

cd /d "%PROJECT_DIR%" || exit /b 1

where docker >nul 2>nul
if errorlevel 1 (
    echo Docker не найден. Установите Docker Desktop и запустите его.
    pause
    exit /b 1
)

echo ==^> Запускаем PostgreSQL и backend...
docker compose up --build -d db app
if errorlevel 1 (
    pause
    exit /b 1
)

echo.
echo ==^> Контейнеры:
docker compose ps

echo.
echo ==^> Проверяем backend:
for /l %%i in (1,1,30) do (
    curl -fsS http://localhost:8080/api/health >nul 2>nul
    if not errorlevel 1 (
        echo Backend доступен: http://localhost:8080
        curl -fsS http://localhost:8080/api/health
        echo.
        goto ready
    )
    timeout /t 1 >nul
)

echo Backend пока не ответил. Проверьте логи: docker compose logs app
pause
exit /b 1

:ready
echo.
echo Тестовые аккаунты:
echo   student_demo / student123
echo   teacher1     / teacher123
echo   admin        / admin123
echo.
pause
