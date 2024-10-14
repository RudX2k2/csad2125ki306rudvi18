#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Define the path to ESP-IDF (update this to your environment if necessary)
export IDF_PATH=~/esp/esp-idf

# Set up ESP-IDF environment (run this command from the ESP-IDF folder)
source $IDF_PATH/export.sh

# Navigate to the project directory
cd ../RPS_Controller

# Remove old build files (optional, ensures clean build)
rm -rf build/

# Build the project using ESP-IDF
idf.py build

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful. Binaries are in the build/ directory."
else
    echo "Build failed. Check errors above."
    exit 1
fi
