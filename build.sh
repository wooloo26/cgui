#!/bin/bash
# Build script for CGUI on Linux/macOS

echo "Building CGUI Demo..."

mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

cd ..

echo ""
echo "Build complete! Run: ./build/cgui_demo"

