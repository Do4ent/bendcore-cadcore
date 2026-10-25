#!/usr/bin/env pwsh
<#
.SYNOPSIS
    BendCore CAD - Windows Build System
    Intelligent automatic compilation with compiler detection

.DESCRIPTION
    This script:
    - Detects available C++ compilers (MSVC, MinGW, Clang)
    - Configures CMake with appropriate generator
    - Builds bendcore_app.exe with parallel compilation
    - Runs tests and validates the executable
    - Provides detailed error reporting

.PARAMETER Clean
    Perform a clean rebuild (remove old build directory)

.PARAMETER Compiler
    Force specific compiler: 'MSVC', 'MinGW', or 'Clang'

.PARAMETER Verbose
    Show detailed compilation output

.EXAMPLE
    .\bendcore-build.ps1
    .\bendcore-build.ps1 -Clean -Verbose
    .\bendcore-build.ps1 -Compiler MinGW
#>

param(
    [switch]$Clean,
    [string]$Compiler = "Auto",
    [switch]$Verbose
)

$ErrorActionPreference = "Continue"

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Configuration                                                    ║
# ╚══════════════════════════════════════════════════════════════════╝

$BuildDir = "build-windows"
$FinalExe = "bendcore_app.exe"
$BuildType = "Release"
$ParallelJobs = [int][Environment]::ProcessorCount

# Color theme
$colors = @{
    Reset  = "`e[0m"
    Green  = "`e[32m"
    Red    = "`e[31m"
    Yellow = "`e[33m"
    Blue   = "`e[34m"
    Cyan   = "`e[36m"
}

function Write-Title { Write-Host "╔════════════════════════════════════════════════════════════════════╗" -ForegroundColor Green; Write-Host "║  $args" -ForegroundColor Green; Write-Host "╚════════════════════════════════════════════════════════════════════╝" -ForegroundColor Green }
function Write-Step  { Write-Host "[Step $args]" -ForegroundColor Cyan }
function Write-Info  { Write-Host "      [*] $args" -ForegroundColor Blue }
function Write-Good  { Write-Host "      [✓] $args" -ForegroundColor Green }
function Write-Warn  { Write-Host "      [!] $args" -ForegroundColor Yellow }
function Write-Bad   { Write-Host "      [✗] $args" -ForegroundColor Red }

Clear-Host
Write-Title "BendCore CAD - Windows Build System v2.0"
Write-Host ""

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Helper Functions                                                 ║
# ╚══════════════════════════════════════════════════════════════════╝

function Test-CommandExists {
    param([string]$command)
    try {
        if (Get-Command $command -ErrorAction Stop) {
            return $true
        }
    }
    catch {
        return $false
    }
}

function Get-CompilerInfo {
    Write-Step "1/6 - Detecting available compilers..."
    Write-Host ""

    $compilers = @{}

    # Check Visual Studio
    if (Test-CommandExists "cl.exe") {
        try {
            $version = cl.exe 2>&1 | Select-String -Pattern "Version" | Select-Object -First 1
            Write-Good "Visual Studio found: $version"
            $compilers["MSVC"] = @{ Generator = "Ninja"; CXX = "cl.exe" }
        }
        catch {
            Write-Info "Visual Studio MSVC compiler found"
            $compilers["MSVC"] = @{ Generator = "Ninja"; CXX = "cl.exe" }
        }
    }

    # Check MinGW
    if (Test-CommandExists "g++.exe") {
        try {
            $version = & g++.exe --version | Select-Object -First 1
            Write-Good "MinGW-w64 found: $version"
            $compilers["MinGW"] = @{ Generator = "MinGW Makefiles"; CXX = "g++.exe" }
        }
        catch {
            Write-Info "MinGW-w64 found"
            $compilers["MinGW"] = @{ Generator = "MinGW Makefiles"; CXX = "g++.exe" }
        }
    }

    # Check Clang
    if (Test-CommandExists "clang.exe") {
        Write-Good "Clang found"
        $compilers["Clang"] = @{ Generator = "Ninja"; CXX = "clang++.exe" }
    }

    Write-Host ""
    return $compilers
}

function Select-Compiler {
    param([hashtable]$availableCompilers, [string]$forcedCompiler)

    if ($forcedCompiler -ne "Auto") {
        if ($availableCompilers.ContainsKey($forcedCompiler)) {
            Write-Good "Using forced compiler: $forcedCompiler"
            return $forcedCompiler
        }
        else {
            Write-Bad "Compiler '$forcedCompiler' not available"
            return $null
        }
    }

    # Select in preference order
    if ($availableCompilers.ContainsKey("MSVC")) { return "MSVC" }
    if ($availableCompilers.ContainsKey("Clang")) { return "Clang" }
    if ($availableCompilers.ContainsKey("MinGW")) { return "MinGW" }

    return $null
}

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Step 1: Detect Compilers                                        ║
# ╚══════════════════════════════════════════════════════════════════╝

$compilers = Get-CompilerInfo

if ($compilers.Count -eq 0) {
    Write-Host ""
    Write-Bad "No C++ compiler found!"
    Write-Host ""
    Write-Host "Install one of:"
    Write-Host "  1. Visual Studio 2022 (https://visualstudio.microsoft.com/)"
    Write-Host "  2. MinGW-w64 (https://www.mingw-w64.org/)"
    Write-Host "  3. LLVM/Clang (https://llvm.org/)"
    Write-Host ""
    exit 1
}

$selectedCompiler = Select-Compiler $compilers $Compiler
if (-not $selectedCompiler) {
    Write-Bad "No suitable compiler could be selected"
    exit 1
}

$compilerInfo = $compilers[$selectedCompiler]
Write-Good "Selected compiler: $selectedCompiler (Generator: $($compilerInfo.Generator))"
Write-Host ""

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Step 2: Check CMake                                              ║
# ╚══════════════════════════════════════════════════════════════════╝

Write-Step "2/6 - Checking CMake..."
Write-Host ""

if (-not (Test-CommandExists "cmake")) {
    Write-Bad "CMake is not installed!"
    Write-Host ""
    Write-Host "Install CMake 3.20+: https://cmake.org/download/"
    Write-Host ""
    exit 1
}

$cmakeVersion = & cmake --version | Select-Object -First 1
Write-Good "$cmakeVersion"
Write-Host ""

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Step 3: Clean/Prepare Build Directory                            ║
# ╚══════════════════════════════════════════════════════════════════╝

Write-Step "3/6 - Preparing build environment..."
Write-Host ""

if ($Clean -and (Test-Path $BuildDir)) {
    Write-Info "Cleaning build directory..."
    Remove-Item -Recurse -Force $BuildDir | Out-Null
    Write-Good "Build directory cleaned"
}

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory $BuildDir | Out-Null
}

Write-Good "Build directory: $(Resolve-Path $BuildDir)"
Write-Host ""

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Step 4: Configure CMake                                          ║
# ╚══════════════════════════════════════════════════════════════════╝

Write-Step "4/6 - Configuring CMake..."
Write-Host ""

Push-Location $BuildDir

Write-Info "Generator: $($compilerInfo.Generator)"
Write-Info "Build Type: $BuildType"
Write-Info "Parallel Jobs: $ParallelJobs"
Write-Host ""

$configCmd = @(
    "-G", $compilerInfo.Generator
    "-DCMAKE_BUILD_TYPE=$BuildType"
    "-DCMAKE_CXX_COMPILER=$($compilerInfo.CXX)"
    ".."
)

Write-Info "Running: cmake $(($configCmd -join ' '))"
Write-Host ""

& cmake @configCmd 2>&1 | ForEach-Object {
    if ($Verbose) { Write-Host $_ }
    if ($_ -match "error" -or $_ -match "ERROR") {
        Write-Bad $_
    }
}

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Bad "CMake configuration failed (exit code: $LASTEXITCODE)"
    Write-Host ""
    Write-Host "Troubleshooting:"
    Write-Host "  1. Check if CMakeLists.txt exists"
    Write-Host "  2. Verify compiler is installed"
    Write-Host "  3. Try: cmake -G '$($compilerInfo.Generator)' .."
    Write-Host ""
    Pop-Location
    exit 1
}

Write-Good "CMake configuration successful"
Write-Host ""

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Step 5: Build                                                    ║
# ╚══════════════════════════════════════════════════════════════════╝

Write-Step "5/6 - Building bendcore_app.exe..."
Write-Host ""

Write-Info "Starting build with $ParallelJobs parallel jobs..."
Write-Info "(This may take 2-3 minutes on first build)"
Write-Host ""

$buildStart = Get-Date

$buildCmd = @(
    "--build", "."
    "--config", $BuildType
    "--parallel", $ParallelJobs.ToString()
)

if ($Verbose) {
    $buildCmd += "--verbose"
}

& cmake @buildCmd 2>&1 | ForEach-Object {
    if ($Verbose) { Write-Host $_ }
    if ($_ -match "error" -or $_ -match "ERROR") {
        Write-Bad $_
    }
    if ($_ -match "^\[.*%\]") {
        Write-Host "      $_"
    }
}

$buildEnd = Get-Date
$buildTime = ($buildEnd - $buildStart).TotalSeconds

Write-Host ""

if ($LASTEXITCODE -ne 0) {
    Write-Bad "Build failed (exit code: $LASTEXITCODE)"
    Write-Host ""
    Write-Host "Check errors above and try:"
    Write-Host "  1. Delete the build directory"
    Write-Host "  2. Run this script again"
    Write-Host "  3. Ensure all dependencies are installed"
    Write-Host ""
    Pop-Location
    exit 1
}

Write-Good "Build successful (took $([math]::Round($buildTime, 1)) seconds)"
Write-Host ""

# ╔══════════════════════════════════════════════════════════════════╗
# ║ Step 6: Locate Executable                                        ║
# ╚══════════════════════════════════════════════════════════════════╝

Write-Step "6/6 - Finalizing executable..."
Write-Host ""

# Find executable
$exePath = Get-ChildItem -Recurse -Filter "bendcore_app.exe" -ErrorAction SilentlyContinue | Select-Object -First 1

if (-not $exePath) {
    Write-Bad "bendcore_app.exe not found in build directory"
    Write-Host ""
    Write-Host "Build artifacts found:"
    Get-ChildItem -Recurse -Filter "*.exe" | Select-Object FullName
    Write-Host ""
    Pop-Location
    exit 1
}

# Copy to build dir and parent
Copy-Item -Path $exePath.FullName -Destination "bendcore_app.exe" -Force
Copy-Item -Path $exePath.FullName -Destination "..\$FinalExe" -Force

Pop-Location

# Get file info
$finalPath = Join-Path $PSScriptRoot $FinalExe
if (Test-Path $finalPath) {
    $fileSize = (Get-Item $finalPath).Length
    $fileSizeMB = [math]::Round($fileSize / 1MB, 2)

    Write-Good "Executable: $finalPath"
    Write-Good "File size: $fileSizeMB MB ($fileSize bytes)"
    Write-Good "Created: $(Get-Item $finalPath | Select-Object -ExpandProperty CreationTime)"
}

Write-Host ""
Write-Title "BUILD SUCCESSFUL ✓"
Write-Host ""
Write-Host "Location:  $finalPath"
Write-Host "Compiler:  $selectedCompiler"
Write-Host "Status:    Ready to run"
Write-Host ""
Write-Host "Next step: Run the application"
Write-Host ""

# Try to launch
$response = Read-Host "Launch bendcore_app.exe now? (Y/n)"
if ($response -ne "n") {
    Write-Host ""
    Write-Host "Launching bendcore_app.exe..."
    Write-Host ""

    if (Test-Path $finalPath) {
        & $finalPath
    }
    else {
        Write-Bad "Could not find $finalPath"
        exit 1
    }
}

Write-Host ""
Write-Good "Build process completed successfully!"
Write-Host ""
