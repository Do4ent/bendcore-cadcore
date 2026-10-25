# BendCore CAD-CORE Build Guide

Complete guide for building BendCore CAD-CORE from source.

## System Requirements

### Minimum Requirements
- GCC 13+ or Clang 15+
- CMake 3.20+
- Node.js 18+
- Python 3.10+

### Recommended
- Ubuntu 22.04 LTS / Debian 12+
- 8+ GB RAM
- SSD with 10GB free space
- Intel/AMD 64-bit CPU

## Building C++ Core

### 1. Install Dependencies

**Ubuntu/Debian**:
```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  git \
  lcov \
  valgrind
```

**macOS**:
```bash
brew install cmake gcc
```

### 2. Configure Build

```bash
cd /tmp/bendcore-cadcore
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Configuration Options**:
```bash
# With coverage reporting
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"

# With memory sanitizer
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address"

# Parallel build (recommended)
cmake --build . --parallel $(nproc)
```

### 3. Build

```bash
# Build (uses all available CPU cores)
cmake --build . --parallel $(nproc)

# Or specify number of jobs
cmake --build . --parallel 8
```

### 4. Run Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R EntityId --output-on-failure

# Show test output immediately
ctest --output-on-failure -V

# Run tests in parallel
ctest -j $(nproc)
```

### 5. Generate Coverage Report

```bash
# Install coverage tools
sudo apt-get install -y lcov

# Build with coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build .
ctest

# Generate report
lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
lcov --remove coverage.info '/usr/*' '/tmp/*' --output-file coverage.info
lcov --summary coverage.info

# Generate HTML report
genhtml coverage.info --output-directory coverage_html
open coverage_html/index.html
```

## Building WebAssembly

### 1. Install Emscripten

```bash
# Clone emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Install latest version
./emsdk install latest
./emsdk activate latest

# Setup environment
source ./emsdk_env.sh
```

### 2. Configure WASM Build

```bash
cd /tmp/bendcore-cadcore
mkdir -p build-wasm
cd build-wasm

# Setup emscripten environment
source /path/to/emsdk/emsdk_env.sh

# Configure CMake for emscripten
emcmake cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -f ../CMakeLists-wasm.txt
```

### 3. Build WASM

```bash
# Build with all CPU cores
emmake make -j$(nproc)

# Or use cmake
cmake --build . --parallel $(nproc)
```

### 4. Verify WASM Output

```bash
# Check generated files
ls -lh bendcore_wasm.{js,wasm}

# File sizes (should be <5MB each)
# bendcore_wasm.js: ~2-3 MB
# bendcore_wasm.wasm: ~2-3 MB
```

## Building React Frontend

### 1. Install Dependencies

```bash
cd web
npm install
# or with yarn
yarn install
```

### 2. Development Build

```bash
# Start development server
npm run dev

# Open http://localhost:5173 in browser
```

### 3. Production Build

```bash
# Build optimized production bundle
npm run build

# Output in: web/dist/

# Verify build
ls -lh dist/
```

### 4. Type Checking

```bash
# Check for TypeScript errors
npm run type-check
```

### 5. Linting

```bash
# Check code style
npm run lint

# Fix issues automatically
npm run lint:fix
```

## Full Build Workflow

### Build Everything for Production

```bash
#!/bin/bash
set -e

PROJECT_DIR="/tmp/bendcore-cadcore"
cd "$PROJECT_DIR"

echo "=== Building C++ Core ==="
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel $(nproc)
ctest --output-on-failure

echo "=== Building WebAssembly ==="
cd ..
source /path/to/emsdk/emsdk_env.sh
mkdir -p build-wasm
cd build-wasm
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -f ../CMakeLists-wasm.txt
emmake make -j$(nproc)
cp bendcore_wasm.* ../web/public/lib/

echo "=== Building React Frontend ==="
cd ../web
npm install
npm run build

echo "=== Build Complete ==="
echo "C++ library: build/"
echo "WebAssembly: build-wasm/"
echo "React bundle: web/dist/"
```

## Troubleshooting Build Issues

### CMake Configuration Errors

```bash
# Clear CMake cache
rm -rf build CMakeCache.txt CMakeFiles

# Reconfigure with verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Compiler Errors

```bash
# Check compiler version
g++ --version  # Should be 13+

# Update compiler
sudo apt-get install -y g++-13
export CC=gcc-13
export CXX=g++-13
```

### Emscripten Issues

```bash
# Update emsdk
cd emsdk
git pull
./emsdk install latest
./emsdk activate latest

# Verify installation
emcc --version
```

### Test Failures

```bash
# Run specific failing test with verbose output
ctest -R <test_name> --output-on-failure -V

# Check test details
ctest --verbose --debug

# Run with memory check
ctest -T MemCheck
```

### WASM Size Issues

```bash
# Check WASM size
du -h build-wasm/bendcore_wasm.wasm

# Optimize WASM
# - Use Release build type
# - Enable aggressive optimizations in CMakeLists-wasm.txt
# - Consider using wasm-opt tool
```

## Performance Tuning

### C++ Build Optimization

```cmake
# In CMakeLists.txt
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")
```

### WASM Optimization

```cmake
# In CMakeLists-wasm.txt
target_link_options(bendcore_wasm.js PRIVATE
    -O3
    -s AGGRESSIVE_VARIABLE_ELIMINATION=1
    -s WASM_OBJECT_FILES=0
)
```

### Parallel Builds

```bash
# Determine optimal job count
nproc  # Shows number of CPU cores

# Build with multiple jobs
cmake --build . --parallel $(nproc)

# Or explicitly set
cmake --build . --parallel 8
```

## Continuous Integration

### Local CI Simulation

```bash
#!/bin/bash
# Simulate GitHub Actions locally

# C++ Debug Build
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel $(nproc)
ctest -C Debug

# C++ Release Build
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel $(nproc)
ctest -C Release

# WebAssembly Build
source /path/to/emsdk/emsdk_env.sh
emcmake cmake -B build-wasm -f CMakeLists-wasm.txt
emmake make -C build-wasm

# React Build
cd web
npm ci
npm run type-check
npm run lint
npm run build
```

## Installation

### System-Wide Installation

```bash
# Build release version
cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
sudo cmake --install build
```

### Local Development Install

```bash
# Install to local directory
cmake -B build \
    -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build
cmake --install build
```

## Docker Build (Optional)

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential cmake git node-npm lcov

WORKDIR /bendcore
COPY . .

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --parallel $(nproc) && \
    ctest --output-on-failure

CMD ["/bin/bash"]
```

Build Docker image:
```bash
docker build -t bendcore-cadcore:latest .
docker run -it bendcore-cadcore:latest
```

## Build Statistics

### Typical Build Times (Release, -j8)
- C++ Core: 20-30 seconds
- WebAssembly: 40-60 seconds
- React: 10-15 seconds
- **Total**: ~2-3 minutes

### Output Sizes
- bendcore library: 500KB
- bendcore_wasm.js: 2-3 MB
- bendcore_wasm.wasm: 2-3 MB
- React bundle: 400-600 KB (gzipped)

## Verification

### Check Build Artifacts

```bash
# C++ library
ls -lh build/libbendcore.a

# WASM modules
ls -lh build-wasm/bendcore_wasm.*

# React bundle
ls -lh web/dist/

# All tests passed
ctest --output-on-failure | grep "100% tests passed"
```

### Run Quality Checks

```bash
# Memory leak check
valgrind ./build/test_entity_id

# Static analysis
cppcheck src/ include/

# Code coverage
lcov --summary build/coverage.info
```

---

**Last Updated**: 2026-10-25  
**Version**: 0.1.0
