@echo off
REM BendCore CAD System - Windows Launcher
REM Professional Tube Bending Designer

setlocal enabledelayedexpansion

REM Check if compiled
if not exist "build\Release\bendcore_app.exe" (
    echo.
    echo [ERROR] bendcore_app.exe not found in build\Release\
    echo.
    echo Please compile first:
    echo   .\bendcore-windows-build.bat
    echo.
    pause
    exit /b 1
)

REM Run application
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║   BendCore CAD System - Professional Tube Bending Designer ║
echo ║              Starting Application...                      ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

build\Release\bendcore_app.exe
pause
