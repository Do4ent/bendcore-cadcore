# BendCore CAD - Windows Build Guide

Complete guide for building `bendcore_app.exe` for Windows - 5 comprehensive methods.

---

## Quick Start (5 minutes)

### Method 1: Automated Build Script (Recommended ⭐)

**Requirements**: Visual Studio 2022, MinGW-w64, or Clang

**Windows Command Prompt**:
```batch
bendcore-auto-build-windows.bat
```

**Windows PowerShell** (Modern):
```powershell
.\bendcore-build.ps1
```

The script will:
- Auto-detect your compiler
- Configure CMake
- Compile bendcore_app.exe
- Run tests
- Launch the application

**Result**: `bendcore_app.exe` in the repository root

---

## Detailed Methods

### Method 2: Manual Build with Visual Studio 2022

**Requirements**:
- Visual Studio 2022 (Community, Professional, or Enterprise)
- C++ development tools workload
- CMake 3.20+

**Steps**:

1. Open Developer Command Prompt for Visual Studio 2022

2. Navigate to repository:
```batch
cd C:\path\to\bendcore-cadcore
```

3. Create build directory:
```batch
mkdir build-windows
cd build-windows
```

4. Configure with CMake:
```batch
cmake -G "Visual Studio 17 2022" -A x64 ..
```

5. Build:
```batch
cmake --build . --config Release --parallel 4
```

6. Find executable:
```batch
dir /s bendcore_app.exe
```

7. Run:
```batch
bendcore_app.exe
```

**Typical build time**: 2-3 minutes

---

### Method 3: Manual Build with MinGW-w64

**Requirements**:
- MinGW-w64 (https://www.mingw-w64.org/)
- CMake 3.20+

**Steps**:

1. Open command prompt with MinGW environment

2. Navigate to repository:
```batch
cd C:\path\to\bendcore-cadcore
```

3. Create build directory:
```batch
mkdir build-windows
cd build-windows
```

4. Configure with CMake:
```batch
cmake -G "MinGW Makefiles" ..
```

5. Build:
```batch
cmake --build . --config Release --parallel 4
```

6. Run:
```batch
bendcore_app.exe
```

**Typical build time**: 3-4 minutes

---

### Method 4: GitHub Actions (CI/CD)

The repository includes automatic Windows builds via GitHub Actions.

**How it works**:
- Every push to `main` branch triggers Windows build
- Both MSVC and MinGW versions are compiled
- Compiled executables are available as artifacts
- Tagged releases are automatically published

**To use**:

1. Check GitHub Actions tab in the repository

2. Download artifacts from recent workflow runs:
   - Go to Actions → Latest Workflow Run
   - Download `bendcore_app-windows-MSVC` artifact

3. Extract and run `bendcore_app.exe`

---

### Method 5: Cross-Compilation from Linux/macOS

**For Linux users** building Windows binaries:

```bash
# Install MinGW-w64
sudo apt-get install mingw-w64

# Create build directory
mkdir build-windows
cd build-windows

# Configure for Windows
cmake -DCMAKE_SYSTEM_NAME=Windows \
       -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
       -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
       -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
       -DCMAKE_FIND_ROOT_PATH=/usr/x86_64-w64-mingw32 \
       -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . --parallel $(nproc)

# Result: bendcore_app.exe in build-windows/Release/
```

---

## System Requirements

### Minimum
- **OS**: Windows 10 or Windows 11
- **Architecture**: x86-64 (Intel/AMD 64-bit)
- **RAM**: 512 MB minimum
- **Disk**: 100 MB free space

### Recommended for Compilation
- **OS**: Windows 10 or Windows 11 (Latest)
- **RAM**: 4+ GB
- **Disk**: 500 MB free space
- **CPU**: 4+ cores recommended

---

## Compiler Installation Guides

### Visual Studio 2022 (Recommended)

1. Download from: https://visualstudio.microsoft.com/
2. Select "Community Edition" (Free)
3. Run installer
4. Choose workload: **"Desktop development with C++"**
5. Click Install

**Time**: ~10-15 minutes depending on internet speed

### MinGW-w64

**Option A: Installer**
1. Download from: https://www.mingw-w64.org/
2. Run installer: `mingw-w64-install.exe`
3. Select: Architecture=x86_64, Threads=posix, Exception=seh
4. Install

**Option B: Portable (No installation)**
1. Download pre-built from: https://sourceforge.net/projects/mingw-w64/
2. Extract to `C:\mingw64`
3. Add to PATH: `C:\mingw64\bin`

**Time**: 5-10 minutes

### CMake

1. Download from: https://cmake.org/download/
2. Run installer: `cmake-x.xx.x-windows-x86_64.msi`
3. Choose: "Add CMake to system PATH"
4. Install

**Time**: 2 minutes

---

## Build Troubleshooting

### Problem: "CMake not found"

**Solution**:
```batch
:: Verify CMake is installed
cmake --version

:: If not installed, download from https://cmake.org/download/
```

### Problem: "Compiler not found"

**Solution**:
```batch
:: Check for Visual Studio
where cl.exe

:: Check for MinGW
where g++.exe

:: If not found, install one of:
:: - Visual Studio 2022: https://visualstudio.microsoft.com/
:: - MinGW-w64: https://www.mingw-w64.org/
```

### Problem: "Build failed - out of memory"

**Solution**:
```batch
:: Reduce parallel jobs
cmake --build . --config Release --parallel 2
```

### Problem: "Permission denied" or file lock errors

**Solution**:
```batch
:: Close all running bendcore_app.exe instances
taskkill /F /IM bendcore_app.exe

:: Clean and rebuild
rmdir /s /q build-windows
mkdir build-windows
cd build-windows
cmake .. && cmake --build . --config Release
```

### Problem: "Long path" errors on Windows

**Solution**:
```batch
:: Enable long paths in Windows
reg add HKLM\SYSTEM\CurrentControlSet\Control\FileSystem /v LongPathsEnabled /t REG_DWORD /d 1 /f

:: Or use shorter path:
cd C:\temp
git clone https://github.com/Do4ent/bendcore-cadcore
cd bendcore-cadcore
bendcore-auto-build-windows.bat
```

---

## Build Verification

### Check executable exists:
```batch
dir /B bendcore_app.exe
```

### Check executable info:
```batch
:: Get file size
dir /S bendcore_app.exe

:: Get version info
wmic datafile where name="C:\path\to\bendcore_app.exe" get Version
```

### Run executable:
```batch
bendcore_app.exe
```

### Expected output:
```
BendCore CAD System - Professional Tube Bending Designer
PRODUCTION READY ✓

✓ Creating Assembly with Constraints...
✓ Direct Editing Operations...
✓ Sheet Metal Operations...
✓ Manufacturing Export (G-Code)...
✓ Bill of Materials...
✓ Quality Inspection...
✓ Visualization Setup...
✓ Advanced Sketcher Tools...
✓ Performance Optimization...

✅ BendCore CAD System - FULLY OPERATIONAL & READY
```

---

## Advanced Build Options

### Build with specific Visual Studio version:

```batch
:: Visual Studio 2019
cmake -G "Visual Studio 16 2019" -A x64 ..

:: Visual Studio 2022
cmake -G "Visual Studio 17 2022" -A x64 ..
```

### Build with custom compiler flags:

```batch
cmake -DCMAKE_CXX_FLAGS="/O2 /W4" ..
```

### Build with sanitizers (Debug):

```batch
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="/RTC1" ..
```

### Build with full optimization:

```batch
cmake -DCMAKE_CXX_FLAGS_RELEASE="/O3 /march=native" ..
```

---

## Continuous Integration / GitHub Actions

The repository includes automated Windows builds.

**Workflow file**: `.github/workflows/windows-build.yml`

**Features**:
- Automatic builds on every commit to `main`
- Both MSVC and MinGW compilation
- Test execution
- Artifact retention for 30 days
- Automatic release creation for tagged commits

**View build status**:
1. Go to GitHub repository
2. Click "Actions" tab
3. Select "Windows Build"
4. View latest workflow run

---

## Development Workflow

### For active development:

```batch
:: Initial setup
mkdir build-windows
cd build-windows
cmake -G "Visual Studio 17 2022" -A x64 ..

:: For each build during development
cmake --build . --config Debug

:: Run executable
.\Debug\bendcore_app.exe
```

### For release builds:

```batch
:: Clean rebuild
rmdir /s /q build-windows
mkdir build-windows
cd build-windows

:: Configure for Release
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ..

:: Build
cmake --build . --config Release --parallel 4

:: Result in: build-windows\Release\bendcore_app.exe
```

---

## Distribution

### Creating a distribution package:

```batch
:: After successful build
mkdir bendcore-release
copy bendcore_app.exe bendcore-release\
copy RUN_ON_WINDOWS.txt bendcore-release\
copy bendcore-windows-build.bat bendcore-release\
copy README.md bendcore-release\

:: Create ZIP
powershell -Command "Compress-Archive -Path bendcore-release -DestinationPath bendcore-windows-release.zip"
```

---

## Additional Resources

- **CMake Documentation**: https://cmake.org/documentation/
- **Visual Studio**: https://visualstudio.microsoft.com/
- **MinGW-w64**: https://www.mingw-w64.org/
- **C++ Reference**: https://en.cppreference.com/

---

## Performance Benchmarks

| Method | Time | Notes |
|--------|------|-------|
| MSVC (first build) | 2-3 min | Parallel compilation, -j4 |
| MSVC (incremental) | 10-30 sec | Cached build |
| MinGW (first build) | 3-4 min | Generally slower than MSVC |
| MinGW (incremental) | 15-45 sec | Depends on changes |

---

## License

BendCore CAD System - See LICENSE file for details

**Last Updated**: October 25, 2026  
**Build System Version**: 2.0  
**Compatible with**: Windows 10, Windows 11
