@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%.."
set "CLIENT_DIR=%PROJECT_DIR%\client-qt\LMSClient"
if "%LMS_QT_BUILD_DIR%"=="" set "LMS_QT_BUILD_DIR=%CLIENT_DIR%\build\one-click-windows"
if "%LMS_API_BASE_URL%"=="" set "LMS_API_BASE_URL=http://localhost:8080"

cd /d "%PROJECT_DIR%" || exit /b 1

echo ==^> Проверяем backend: %LMS_API_BASE_URL%
curl -fsS "%LMS_API_BASE_URL%/api/health" >nul 2>nul
if errorlevel 1 (
    echo Backend не отвечает. Сначала запустите scripts\start-server.bat.
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo CMake не найден. Установите CMake и Qt либо откройте проект в Qt Creator.
    pause
    exit /b 1
)

echo.
echo ==^> Собираем Qt-клиент...
cmake -S "%CLIENT_DIR%" -B "%LMS_QT_BUILD_DIR%"
if errorlevel 1 (
    echo Не удалось сконфигурировать Qt-клиент.
    pause
    exit /b 1
)

cmake --build "%LMS_QT_BUILD_DIR%" --config Debug
if errorlevel 1 (
    echo Не удалось собрать Qt-клиент.
    pause
    exit /b 1
)

set "APP_BIN=%LMS_QT_BUILD_DIR%\Debug\LMSClient.exe"
if not exist "%APP_BIN%" set "APP_BIN=%LMS_QT_BUILD_DIR%\LMSClient.exe"

if not exist "%APP_BIN%" (
    echo Не удалось найти LMSClient.exe в %LMS_QT_BUILD_DIR%
    pause
    exit /b 1
)

echo.
echo ==^> Запускаем Qt-клиент...
set "LMS_API_BASE_URL=%LMS_API_BASE_URL%"
"%APP_BIN%"
pause
