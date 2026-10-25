# Installation Guide

## System Requirements

### Minimum Requirements
- **OS**: Linux (Ubuntu 20.04+), Windows 10+, macOS 10.15+
- **Processor**: Intel/AMD x86-64, 2+ cores
- **RAM**: 4GB minimum, 8GB recommended
- **Storage**: 500MB for build, 200MB for runtime

### Build Requirements
- **CMake**: 3.20 or higher
- **C++ Compiler**: GCC 11+, Clang 12+, or MSVC 2019+
- **Node.js**: 16+ (for web frontend)
- **Emscripten**: Latest (for WebAssembly builds)

## Linux Installation

### Ubuntu/Debian

1. **Install dependencies**:
```bash
sudo apt-get update
sudo apt-get install -y \
  cmake \
  g++ \
  build-essential \
  git \
  curl \
  nodejs npm
```

2. **Clone repository**:
```bash
git clone https://github.com/Do4ent/bendcore-cadcore.git
cd bendcore-cadcore
```

3. **Build C++ core**:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel $(nproc)
```

4. **Run tests**:
```bash
ctest --output-on-failure
```

5. **Run application**:
```bash
./bendcore_app
```

### Building WebAssembly

```bash
# Install Emscripten
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# Build WASM
cd /path/to/bendcore-cadcore
mkdir build-wasm
cd build-wasm
emcmake cmake .. -f ../CMakeLists-wasm.txt
emmake make -j$(nproc)
```

### Building React Frontend

```bash
cd web
npm install
npm run build
npm run dev
```

## Windows Installation

1. **Install Visual Studio Community** with C++ development tools
2. **Install CMake** from https://cmake.org/download/
3. **Install Node.js** from https://nodejs.org/
4. **Clone repository**:
```bash
git clone https://github.com/Do4ent/bendcore-cadcore.git
cd bendcore-cadcore
```

5. **Build using CMake GUI or command line**:
```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

6. **See RUN_ON_WINDOWS.txt for detailed Windows instructions**

## macOS Installation

```bash
# Install Homebrew if not installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake gcc node

# Clone and build
git clone https://github.com/Do4ent/bendcore-cadcore.git
cd bendcore-cadcore
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel $(sysctl -n hw.ncpu)
ctest --output-on-failure
```

## Docker Build

```bash
docker build -t bendcore:latest .
docker run -it bendcore:latest
```

## Verification

After building, verify the installation:

```bash
# Check binary
./bendcore_app --version

# Run all tests
ctest --output-on-failure --parallel $(nproc)

# Expected output: 50 tests PASSED
```

## Troubleshooting

### CMake not found
- Linux: `sudo apt-get install cmake`
- macOS: `brew install cmake`
- Windows: Download from https://cmake.org/download/

### Compilation errors
- Ensure C++20 support: GCC 11+, Clang 12+, or MSVC 2019+
- Update compiler: `sudo apt-get install g++`

### Test failures
- Clean build: `rm -rf build && mkdir build && cd build`
- Run with verbose: `ctest --output-on-failure -VV`

### WebAssembly issues
- Update Emscripten: `./emsdk update && ./emsdk activate latest`
- Check Node.js version: `node --version` (should be 16+)

## Next Steps

1. Read [ARCHITECTURE.md](ARCHITECTURE.md) for system overview
2. Check [BUILD.md](../BUILD.md) for detailed build options
3. Review [examples/](../examples/) for usage examples
4. See [INTEGRATION.md](../INTEGRATION.md) for API documentation
