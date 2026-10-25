#!/bin/bash

# BendCore CAD - Test Script
# Runs all tests with various options

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
VERBOSE="${VERBOSE:-0}"
COVERAGE="${COVERAGE:-0}"

echo "========================================="
echo "BendCore CAD - Test Suite"
echo "========================================="

# Check if build directory exists
if [ ! -d "$PROJECT_ROOT/build" ]; then
    echo "Error: build directory not found"
    echo "Please run: ./scripts/build.sh"
    exit 1
fi

cd "$PROJECT_ROOT/build"

# Run tests
echo ""
echo "Running tests..."
if [ "$VERBOSE" -eq 1 ]; then
    ctest --output-on-failure -VV --parallel $(nproc)
else
    ctest --output-on-failure --parallel $(nproc)
fi

# Generate coverage report (optional)
if [ "$COVERAGE" -eq 1 ]; then
    echo ""
    echo "Generating coverage report..."

    if ! command -v lcov &> /dev/null; then
        echo "Error: lcov not found"
        echo "Please install: sudo apt-get install lcov"
        exit 1
    fi

    lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
    lcov --remove coverage.info '/usr/*' '/tmp/*' --output-file coverage.info --rc lcov_branch_coverage=1

    echo ""
    lcov --summary coverage.info

    # Generate HTML report
    genhtml coverage.info --output-directory coverage-report
    echo "Coverage report generated in: coverage-report/index.html"
fi

echo ""
echo "========================================="
echo "✓ Tests completed!"
echo "========================================="
