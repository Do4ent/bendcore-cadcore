#!/bin/bash

# BendCore CAD - Clean Script
# Removes build artifacts and temporary files

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "Cleaning build artifacts..."

# Remove build directories
rm -rf "$PROJECT_ROOT/build"
rm -rf "$PROJECT_ROOT/build-wasm"
rm -rf "$PROJECT_ROOT/web/dist"
rm -rf "$PROJECT_ROOT/web/node_modules"

# Remove temporary files
rm -rf "$PROJECT_ROOT/.cmake"
find "$PROJECT_ROOT" -type d -name "CMakeFiles" -exec rm -rf {} + 2>/dev/null || true
find "$PROJECT_ROOT" -name "*.o" -delete
find "$PROJECT_ROOT" -name "*.a" -delete
find "$PROJECT_ROOT" -name "CMakeCache.txt" -delete

# Remove coverage files
rm -rf "$PROJECT_ROOT/coverage-report"
rm -f "$PROJECT_ROOT/build/coverage.info"

echo "✓ Cleaned successfully"
