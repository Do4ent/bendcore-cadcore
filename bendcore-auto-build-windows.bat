@echo off
REM ╔══════════════════════════════════════════════════════════════════╗
REM ║  BendCore CAD - Windows Automatic Build System                  ║
REM ║  Intelligent cross-platform compilation with auto-detection      ║
REM ╚══════════════════════════════════════════════════════════════════╝

setlocal enabledelayedexpansion
color 0A

cls
echo.
echo ╔══════════════════════════════════════════════════════════════════╗
echo ║                                                                  ║
echo ║  BendCore CAD - Windows Automatic Build System v2.0             ║
echo ║  Professional Tube Bending Design System                        ║
echo ║                                                                  ║
echo ╚══════════════════════════════════════════════════════════════════╝
echo.

REM Global settings
set "BUILD_DIR=build-windows"
set "FINAL_EXE=bendcore_app.exe"
set "BUILD_TYPE=Release"
set "PARALLEL_JOBS=4"
set "DETECTED_COMPILER="
set "GENERATOR="
set "EXIT_CODE=0"

REM ╔══════════════════════════════════════════════════════════════════╗
REM ║ Step 1: Detect Available Compilers                               ║
REM ╚══════════════════════════════════════════════════════════════════╝

echo [Step 1/6] Detecting available compilers...
echo.

set "MSVC_FOUND=0"
set "MINGW_FOUND=0"

REM Check for Visual Studio 2022/2019
where cl.exe >nul 2>&1
if !errorlevel! equ 0 (
    echo      [+] Visual Studio C++ Compiler found
    set "MSVC_FOUND=1"
    set "DETECTED_COMPILER=MSVC"
    set "GENERATOR=Visual Studio 17 2022"
)

REM Check for MinGW-w64
where g++.exe >nul 2>&1
if !errorlevel! equ 0 (
    echo      [+] MinGW-w64 found
    set "MINGW_FOUND=1"
    if "!DETECTED_COMPILER!"=="" (
        set "DETECTED_COMPILER=MinGW"
        set "GENERATOR=MinGW Makefiles"
    )
)

REM Check for clang
where clang.exe >nul 2>&1
if !errorlevel! equ 0 (
    echo      [+] Clang found
)

echo.

REM Display compiler options
if !MSVC_FOUND! equ 1 (
    echo      Available Compilers:
    echo        1. Visual Studio 2022 (Recommended for Windows)
    if !MINGW_FOUND! equ 1 echo        2. MinGW-w64
) else if !MINGW_FOUND! equ 1 (
    echo      Available Compilers:
    echo        1. MinGW-w64
) else (
    color 0C
    echo.
    echo ╔══════════════════════════════════════════════════════════════════╗
    echo ║  ERROR: No supported C++ compiler found!                         ║
    echo ╚══════════════════════════════════════════════════════════════════╝
    echo.
    echo Install one of the following:
    echo.
    echo  [1] Visual Studio 2022
    echo      https://visualstudio.microsoft.com/
    echo      Download: Community Edition (Free)
    echo      Install: C++ Development Tools workload
    echo.
    echo  [2] MinGW-w64 (Portable)
    echo      https://www.mingw-w64.org/
    echo      Download: x86_64-win32-seh
    echo.
    echo  [3] LLVM/Clang
    echo      https://llvm.org/
    echo.
    pause
    exit /b 1
)

echo.
echo      Using: !DETECTED_COMPILER! (!GENERATOR!)
echo.

REM ╔══════════════════════════════════════════════════════════════════╗
REM ║ Step 2: Check CMake Installation                                 ║
REM ╚══════════════════════════════════════════════════════════════════╝

echo [Step 2/6] Checking CMake installation...
echo.

cmake --version >nul 2>&1
if !errorlevel! neq 0 (
    color 0C
    echo.
    echo ╔══════════════════════════════════════════════════════════════════╗
    echo ║  ERROR: CMake is not installed!                                  ║
    echo ╚══════════════════════════════════════════════════════════════════╝
    echo.
    echo Install CMake 3.20 or later:
    echo   https://cmake.org/download/
    echo.
    pause
    exit /b 1
)

for /f "tokens=3" %%i in ('cmake --version') do (
    echo      CMake version: %%i
    goto :check_cmake_done
)
:check_cmake_done

echo.

REM ╔══════════════════════════════════════════════════════════════════╗
REM ║ Step 3: Clean Build Directory                                    ║
REM ╚══════════════════════════════════════════════════════════════════╝

echo [Step 3/6] Preparing build environment...
echo.

if exist "!BUILD_DIR!" (
    echo      Cleaning old build directory...
    rmdir /s /q "!BUILD_DIR!" >nul 2>&1
)

mkdir "!BUILD_DIR!"
echo      Created build directory: !BUILD_DIR!
echo.

REM ╔══════════════════════════════════════════════════════════════════╗
REM ║ Step 4: Configure CMake                                          ║
REM ╚══════════════════════════════════════════════════════════════════╝

echo [Step 4/6] Configuring CMake...
echo.

pushd "!BUILD_DIR!"

echo      Generator: !GENERATOR!
echo      Build Type: !BUILD_TYPE!
echo      Parallel Jobs: !PARALLEL_JOBS!
echo.

if "!DETECTED_COMPILER!"=="MSVC" (
    echo      Running: cmake -G "!GENERATOR!" -A x64 -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ..
    cmake -G "!GENERATOR!" -A x64 -DCMAKE_BUILD_TYPE=!BUILD_TYPE! .. 2>&1
) else (
    echo      Running: cmake -G "!GENERATOR!" -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ..
    cmake -G "!GENERATOR!" -DCMAKE_BUILD_TYPE=!BUILD_TYPE! .. 2>&1
)

if !errorlevel! neq 0 (
    color 0C
    echo.
    echo ╔══════════════════════════════════════════════════════════════════╗
    echo ║  ERROR: CMake configuration failed!                              ║
    echo ╚══════════════════════════════════════════════════════════════════╝
    echo.
    echo Troubleshooting:
    echo   1. Check if CMakeLists.txt exists in parent directory
    echo   2. Verify compiler is properly installed
    echo   3. Try manually: cmake -G "!GENERATOR!" ..
    echo.
    popd
    pause
    exit /b 1
)

echo      ✓ CMake configuration successful
echo.
popd

REM ╔══════════════════════════════════════════════════════════════════╗
REM ║ Step 5: Compile                                                  ║
REM ╚══════════════════════════════════════════════════════════════════╝

echo [Step 5/6] Compiling bendcore_app.exe...
echo           (This may take 2-3 minutes on first build)
echo.

pushd "!BUILD_DIR!"

REM Show build progress
echo      Building with !PARALLEL_JOBS! parallel jobs...
echo.

REM Get start time for timing info
for /f "tokens=1-4 delims=:.," %%a in ('powershell Get-Date ^| powershell {$_.ToString(\"HH:mm:ss\")}') do set "START_TIME=%%a:%%b:%%c"

if "!DETECTED_COMPILER!"=="MSVC" (
    cmake --build . --config !BUILD_TYPE! --parallel !PARALLEL_JOBS! 2>&1
) else (
    cmake --build . --config !BUILD_TYPE! --parallel !PARALLEL_JOBS! 2>&1
)

set "BUILD_RESULT=!errorlevel!"

REM Get end time
for /f "tokens=1-4 delims=:.," %%a in ('powershell Get-Date ^| powershell {$_.ToString(\"HH:mm:ss\")}') do set "END_TIME=%%a:%%b:%%c"

echo.

if !BUILD_RESULT! neq 0 (
    color 0C
    echo ╔══════════════════════════════════════════════════════════════════╗
    echo ║  ERROR: Build failed!                                            ║
    echo ╚══════════════════════════════════════════════════════════════════╝
    echo.
    echo Check the error messages above and try:
    echo   1. Delete build-windows folder completely
    echo   2. Run this script again
    echo   3. Check that all dependencies are installed
    echo.
    popd
    pause
    exit /b 1
)

echo      ✓ Compilation successful (Started: !START_TIME!, Ended: !END_TIME!)
echo.

REM ╔══════════════════════════════════════════════════════════════════╗
REM ║ Step 6: Locate and Copy Executable                               ║
REM ╚══════════════════════════════════════════════════════════════════╝

echo [Step 6/6] Finalizing executable...
echo.

REM Find the executable in build directory
set "FOUND_EXE="
for /f "delims=" %%F in ('dir /b /s "bendcore_app.exe" 2^>nul') do (
    set "FOUND_EXE=%%F"
    goto :found_exe
)

:found_exe
if defined FOUND_EXE (
    echo      Found: !FOUND_EXE!
    copy /y "!FOUND_EXE!" "bendcore_app.exe" >nul
    copy /y "bendcore_app.exe" "..\!FINAL_EXE!" >nul
    echo      Copied to: ..\\!FINAL_EXE!

    REM Get file size
    for /f %%A in ('dir /b "..\!FINAL_EXE!"') do (
        echo      Size: %%~zA bytes
    )
) else (
    echo.
    color 0C
    echo ╔══════════════════════════════════════════════════════════════════╗
    echo ║  ERROR: bendcore_app.exe not found in build directory!           ║
    echo ╚══════════════════════════════════════════════════════════════════╝
    echo.
    echo Contents of build directory:
    dir /s "*.exe"
    popd
    pause
    exit /b 1
)

popd
echo.

REM ╔══════════════════════════════════════════════════════════════════╗
REM ║ Build Complete                                                   ║
REM ╚══════════════════════════════════════════════════════════════════╝

color 0A
echo.
echo ╔══════════════════════════════════════════════════════════════════╗
echo ║  BUILD SUCCESSFUL ✓                                             ║
echo ╚══════════════════════════════════════════════════════════════════╝
echo.
echo Program:       !FINAL_EXE!
echo Location:      %CD%\!FINAL_EXE!
echo Compiler:      !DETECTED_COMPILER!
echo Status:        Ready to run
echo.
echo Next step:     Run: !FINAL_EXE!
echo.
pause

REM Try to run the app
echo.
echo Launching bendcore_app.exe...
echo.

if exist "!FINAL_EXE!" (
    start "" "!FINAL_EXE!"
) else (
    color 0C
    echo ERROR: Could not find !FINAL_EXE! in current directory
    pause
    exit /b 1
)

endlocal
exit /b 0
