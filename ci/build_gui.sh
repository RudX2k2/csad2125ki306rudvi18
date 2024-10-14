#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

cd ../RPS_GUI/

# Create build directory if it doesn't exist
mkdir -p build

# Move into build directory
cd build/

# Configure the project with CMake (ensure Qt is installed and available)
cmake ..

# Build the project
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Qt project build successful. Binaries are in the build/ directory."
else
    echo "Build failed. Check errors above."
    exit 1
fi

