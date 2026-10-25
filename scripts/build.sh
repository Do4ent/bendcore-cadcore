#!/bin/bash

# BendCore CAD - Build Script
# Builds the C++ core and optional WebAssembly/Web components

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_TYPE="${1:-Release}"
BUILD_WASM="${BUILD_WASM:-0}"
BUILD_WEB="${BUILD_WEB:-0}"

echo "========================================="
echo "BendCore CAD - Build Script"
echo "========================================="
echo "Project Root: $PROJECT_ROOT"
echo "Build Type: $BUILD_TYPE"
echo "Build WASM: $BUILD_WASM"
echo "Build Web: $BUILD_WEB"
echo "========================================="

# Build C++ Core
echo ""
echo "[1/3] Building C++ Core..."
cd "$PROJECT_ROOT"
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
cmake --build . --config "$BUILD_TYPE" --parallel $(nproc)

echo "✓ C++ Core build complete"

# Run tests
echo ""
echo "[2/3] Running Tests..."
ctest --output-on-failure --parallel $(nproc)
echo "✓ All tests passed"

# Build WebAssembly (optional)
if [ "$BUILD_WASM" -eq 1 ]; then
    echo ""
    echo "[3a/3] Building WebAssembly..."

    # Check if emsdk is activated
    if ! command -v emcmake &> /dev/null; then
        echo "Error: Emscripten SDK not activated"
        echo "Please run: source emsdk/emsdk_env.sh"
        exit 1
    fi

    cd "$PROJECT_ROOT"
    mkdir -p build-wasm
    cd build-wasm

    emcmake cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -f ../CMakeLists-wasm.txt
    emmake make -j$(nproc)

    echo "✓ WebAssembly build complete"
fi

# Build Web (optional)
if [ "$BUILD_WEB" -eq 1 ]; then
    echo ""
    echo "[3b/3] Building Web Frontend..."

    if ! command -v npm &> /dev/null; then
        echo "Error: npm not found"
        echo "Please install Node.js from https://nodejs.org/"
        exit 1
    fi

    cd "$PROJECT_ROOT/web"
    npm install
    npm run build

    echo "✓ Web frontend build complete"
fi

echo ""
echo "========================================="
echo "✓ Build completed successfully!"
echo "========================================="
echo ""
echo "To run the application:"
echo "  ./build/bendcore_app"
echo ""
echo "To run tests:"
echo "  cd build && ctest --output-on-failure"
echo ""
