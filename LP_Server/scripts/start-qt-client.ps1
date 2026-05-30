$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectDir = Resolve-Path (Join-Path $scriptDir "..")
$clientDir = Join-Path $projectDir "client-qt/LMSClient"
$buildDir = if ($env:LMS_QT_BUILD_DIR) { $env:LMS_QT_BUILD_DIR } else { Join-Path $clientDir "build/one-click-windows" }
$apiUrl = if ($env:LMS_API_BASE_URL) { $env:LMS_API_BASE_URL } else { "http://localhost:8080" }

Set-Location $projectDir

Write-Host "==> Проверяем backend: $apiUrl"
try {
    Invoke-WebRequest -Uri "$apiUrl/api/health" -UseBasicParsing -TimeoutSec 2 | Out-Null
} catch {
    Write-Host "Backend не отвечает. Сначала запустите scripts/start-server.ps1."
}

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "CMake не найден. Установите CMake и Qt либо откройте проект в Qt Creator."
    Read-Host "Нажмите Enter для выхода"
    exit 1
}

Write-Host ""
Write-Host "==> Собираем Qt-клиент..."
cmake -S $clientDir -B $buildDir
cmake --build $buildDir --config Debug

$candidates = @(
    (Join-Path $buildDir "Debug/LMSClient.exe"),
    (Join-Path $buildDir "LMSClient.exe")
)

$appBin = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $appBin) {
    Write-Host "Не удалось найти LMSClient.exe в $buildDir"
    Read-Host "Нажмите Enter для выхода"
    exit 1
}

Write-Host ""
Write-Host "==> Запускаем Qt-клиент..."
$env:LMS_API_BASE_URL = $apiUrl
& $appBin
Read-Host "Нажмите Enter для выхода"
