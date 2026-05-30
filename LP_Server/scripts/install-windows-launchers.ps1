$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectDir = Resolve-Path (Join-Path $scriptDir "..")
$desktop = [Environment]::GetFolderPath("Desktop")
$iconPath = Join-Path $projectDir "client-qt\LMSClient\resources\icons\lms-app.ico"
$fallbackIcon = "$env:SystemRoot\System32\shell32.dll,13"

$shell = New-Object -ComObject WScript.Shell

$serverShortcut = $shell.CreateShortcut((Join-Path $desktop "LMS Server.lnk"))
$serverShortcut.TargetPath = "powershell.exe"
$serverShortcut.Arguments = "-ExecutionPolicy Bypass -File `"$projectDir\scripts\start-server.ps1`""
$serverShortcut.WorkingDirectory = $projectDir
$serverShortcut.IconLocation = if (Test-Path $iconPath) { $iconPath } else { $fallbackIcon }
$serverShortcut.Save()

$clientShortcut = $shell.CreateShortcut((Join-Path $desktop "LMS Client.lnk"))
$clientShortcut.TargetPath = "powershell.exe"
$clientShortcut.Arguments = "-ExecutionPolicy Bypass -File `"$projectDir\scripts\start-qt-client.ps1`""
$clientShortcut.WorkingDirectory = $projectDir
$clientShortcut.IconLocation = if (Test-Path $iconPath) { $iconPath } else { $fallbackIcon }
$clientShortcut.Save()

Write-Host "Windows launchers created on Desktop."
