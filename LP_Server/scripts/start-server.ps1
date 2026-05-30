$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectDir = Resolve-Path (Join-Path $scriptDir "..")
Set-Location $projectDir

if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Host "Docker не найден. Установите Docker Desktop и запустите его."
    Read-Host "Нажмите Enter для выхода"
    exit 1
}

Write-Host "==> Запускаем PostgreSQL и backend..."
docker compose up --build -d db app

Write-Host ""
Write-Host "==> Контейнеры:"
docker compose ps

Write-Host ""
Write-Host "==> Проверяем backend:"
$ready = $false
for ($i = 1; $i -le 30; $i++) {
    try {
        $health = Invoke-WebRequest -Uri "http://localhost:8080/api/health" -UseBasicParsing -TimeoutSec 2
        Write-Host "Backend доступен: http://localhost:8080"
        Write-Host $health.Content
        $ready = $true
        break
    } catch {
        Start-Sleep -Seconds 1
    }
}

if (-not $ready) {
    Write-Host "Backend пока не ответил. Проверьте логи: docker compose logs app"
    Read-Host "Нажмите Enter для выхода"
    exit 1
}

Write-Host ""
Write-Host "Тестовые аккаунты:"
Write-Host "  student_demo / student123"
Write-Host "  teacher1     / teacher123"
Write-Host "  admin        / admin123"
Read-Host "Нажмите Enter для выхода"
