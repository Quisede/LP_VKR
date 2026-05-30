$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectDir = Resolve-Path (Join-Path $scriptDir "..")
$clientDir = Join-Path $projectDir "client-qt\LMSClient"
$buildDir = if ($env:LMS_QT_BUILD_DIR) { $env:LMS_QT_BUILD_DIR } else { Join-Path $clientDir "build\package-windows" }
$distDir = Join-Path $projectDir "dist\windows"
$deployDir = Join-Path $distDir "LMSClient"

New-Item -ItemType Directory -Force -Path $buildDir, $deployDir | Out-Null

cmake -S $clientDir -B $buildDir -DCMAKE_BUILD_TYPE=Release
cmake --build $buildDir --config Release

$exeCandidates = @(
    (Join-Path $buildDir "Release\LMSClient.exe"),
    (Join-Path $buildDir "LMSClient.exe")
)
$exe = $exeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $exe) {
    throw "LMSClient.exe not found in $buildDir"
}

Copy-Item $exe $deployDir -Force

if (Get-Command windeployqt -ErrorAction SilentlyContinue) {
    windeployqt (Join-Path $deployDir "LMSClient.exe")
} else {
    Write-Warning "windeployqt not found. Qt runtime files were not copied."
}

$issPath = Join-Path $distDir "lms-client.iss"
@"
[Setup]
AppName=LMS Client
AppVersion=1.0.0
DefaultDirName={autopf}\LMS Client
DefaultGroupName=LMS Client
OutputDir=$distDir
OutputBaseFilename=LMSClientSetup
Compression=lzma
SolidCompression=yes

[Files]
Source: "$deployDir\*"; DestDir: "{app}"; Flags: recursesubdirs

[Icons]
Name: "{group}\LMS Client"; Filename: "{app}\LMSClient.exe"
Name: "{commondesktop}\LMS Client"; Filename: "{app}\LMSClient.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional icons:"
"@ | Set-Content -Encoding UTF8 $issPath

if (Get-Command iscc -ErrorAction SilentlyContinue) {
    iscc $issPath
    Write-Host "Windows installer is ready: $distDir\LMSClientSetup.exe"
} else {
    Write-Warning "Inno Setup compiler 'iscc' not found. Deployment folder is ready: $deployDir"
    Write-Host "Installer script generated: $issPath"
}
