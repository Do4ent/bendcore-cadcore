@echo off
REM BendCore CAD System - Windows Build Script
REM Automatically compiles for Windows with Visual Studio or MinGW

echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║   BendCore CAD System - Windows Build Script               ║
echo ║   Professional Tube Bending Designer                       ║
echo ╚════════════════════════════════════════════════════════════╝
echo.

setlocal enabledelayedexpansion

REM Check for Visual Studio 2022
echo [1/4] Checking for Visual Studio 2022...
set "VS_PATH="
for /f "tokens=*" %%i in ('where cl.exe 2^>nul') do set "VS_FOUND=1"

if defined VS_FOUND (
    echo      ✓ Visual Studio 2022 found
    set "GENERATOR=Visual Studio 17 2022"
    set "ARCH=x64"
    goto :compile
)

REM Check for MinGW
echo [1/4] Checking for MinGW-w64...
set "MINGW_FOUND="
for /f "tokens=*" %%i in ('where g++.exe 2^>nul') do set "MINGW_FOUND=1"

if defined MINGW_FOUND (
    echo      ✓ MinGW-w64 found
    set "GENERATOR=MinGW Makefiles"
    goto :compile
)

echo.
echo [ERROR] Neither Visual Studio 2022 nor MinGW found!
echo.
echo Install one of:
echo   • Visual Studio 2022 (C++ Workload): https://visualstudio.microsoft.com/
echo   • MinGW-w64: https://www.mingw-w64.org/
echo.
pause
exit /b 1

:compile
echo.
echo [2/4] Creating build directory...
if exist build-windows rmdir /s /q build-windows
mkdir build-windows
cd build-windows

echo.
echo [3/4] Configuring CMake...
if "%GENERATOR%"=="Visual Studio 17 2022" (
    cmake -G "%GENERATOR%" -A x64 .. || goto :error
) else (
    cmake -G "%GENERATOR%" .. || goto :error
)

echo.
echo [4/4] Compiling (this may take 2-3 minutes)...
cmake --build . --config Release -j 4 || goto :error

echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║   ✅ BUILD SUCCESSFUL!                                     ║
echo ╚════════════════════════════════════════════════════════════╝
echo.
echo Executable: build-windows\Release\bendcore_app.exe
echo.
pause
cd ..
exit /b 0

:error
echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║   ❌ BUILD FAILED                                          ║
echo ╚════════════════════════════════════════════════════════════╝
echo.
pause
cd ..
exit /b 1
